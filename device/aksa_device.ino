/* The board as the whole computer: it broadcasts its own Wi-Fi hotspot,
   serves a tiny editor page, and runs submitted programs on-chip.
   Join "Aksa-XXXX", open http://192.168.4.1, type code, tap Run.

   The program runs in its own task (aksaTask) so the server stays responsive:
   the page streams printed text live via GET /output and can end a run with
   POST /stop — which is what lets an interactive `selama (benar)` loop (poll a
   button, print on change) run for as long as you want. */

#include <WebServer.h>
#include <WiFi.h>

extern "C" {
#include "vm.h"
}

#include "aksa_locale.h"
#include "index_html.h"

extern const AksaHost device_host;
void device_out_begin();
void device_request_stop();
void device_out_write(const char *t);
size_t device_out_drain(char *buf, size_t n);

static const size_t MAX_SOURCE = 16 * 1024;
static const uint32_t AKSA_STACK =
    48 * 1024; /* front-end recurses; sized so a program
                  at PARSE_DEPTH_MAX (~30KB worst case,
                  measured) can't overflow into a reboot */

static WebServer server(80);
static AksaLocale loc_id, loc_en;

static volatile bool running;
static char *run_src; /* heap copy the task owns and frees */
static const AksaLocale *run_loc;

static void aksaTask(void *) {
  AksaErrors errs = {0};
  aksa_run(run_src, run_loc, &errs, &device_host);
  if (errs.count > 0) {
    size_t cap = 64, len = 0;
    char *msgs = (char *)malloc(cap);
    if (msgs) {
      msgs[0] = 0;
      aksa_errors_dump(run_loc, &errs, &msgs, &len, &cap);
      device_out_write(msgs);
      free(msgs);
    }
  }
  /* Min free stack over this run (bytes). Compare to AKSA_STACK to see real
     headroom; the low point is a deeply nested program. Safe to remove. */
  Serial.printf("aksa stack free min %u / %u bytes\n",
                (unsigned)uxTaskGetStackHighWaterMark(NULL),
                (unsigned)AKSA_STACK);
  free(run_src);
  run_src = NULL;
  running = false;
  vTaskDelete(NULL);
}

static void handleRun() {
  String src = server.arg("plain");
  if (src.length() == 0 || src.length() > MAX_SOURCE) {
    server.send(400, "text/plain; charset=utf-8", "bad request");
    return;
  }
  if (running) { /* preempt: stop the old program, wait for it to end */
    device_request_stop();
    for (int i = 0; i < 200 && running; i++)
      delay(10); /* up to ~2 s */
    if (running) {
      server.send(409, "text/plain", "busy");
      return;
    }
  }

  run_loc = server.arg("locale") == "en" ? &loc_en : &loc_id;
  run_src = strdup(src.c_str());
  if (!run_src) {
    server.send(500, "text/plain; charset=utf-8", "out of memory");
    return;
  }
  device_out_begin();
  running = true;
  if (xTaskCreate(aksaTask, "aksa", AKSA_STACK, nullptr, 1, nullptr) !=
      pdPASS) {
    running = false; /* else every future /run wedges on 409 busy */
    free(run_src);
    run_src = NULL;
    server.send(500, "text/plain; charset=utf-8", "out of memory");
    return;
  }
  server.send(200, "text/plain; charset=utf-8", "ok");
  Serial.printf("run started, free heap %u\n", ESP.getFreeHeap());
}

static void handleOutput() {
  char buf[513];
  size_t n = device_out_drain(buf, sizeof buf - 1);
  buf[n] = 0;
  server.sendHeader("X-Running", running ? "1" : "0");
  server.send(200, "text/plain; charset=utf-8", buf);
}

static void handleStop() {
  device_request_stop();
  server.send(200, "text/plain; charset=utf-8", "ok");
}

void setup() {
  Serial.begin(115200);
  setCpuFrequencyMhz(80); /* 160->80 (WiFi's floor): ~half the CPU heat, plenty
                             for serving the page + running programs */

  char err[128];
  if (aksa_locale_load(&loc_id, aksa_locale_id, err, sizeof err))
    Serial.printf("locale id: %s\n", err);
  if (aksa_locale_load(&loc_en, aksa_locale_en, err, sizeof err))
    Serial.printf("locale en: %s\n", err);

  char ssid[16];
  snprintf(ssid, sizeof ssid, "Aksa-%04X",
           (unsigned)(ESP.getEfuseMac() >> 32) & 0xFFFF);
  /* password = SSID (9 chars, meets WPA2's 8-char minimum); max 1 client so
     two kids can't stomp on each other's run / output on the one board. */
  WiFi.softAP(ssid, ssid, 1, 0, 1);
  WiFi.setTxPower(WIFI_POWER_8_5dBm); /* one client sitting right next to the
                                         board; full-range TX just adds heat */

  server.on("/", HTTP_GET,
            [] { server.send_P(200, "text/html", AKSA_INDEX_HTML); });
  server.on("/run", HTTP_POST, handleRun);
  server.on("/output", HTTP_GET, handleOutput);
  server.on("/stop", HTTP_POST, handleStop);
  server.begin();
  Serial.printf("%s up at http://%s, free heap %u\n", ssid,
                WiFi.softAPIP().toString().c_str(), ESP.getFreeHeap());
}

void loop() {
  server.handleClient();
  delay(1);
}
