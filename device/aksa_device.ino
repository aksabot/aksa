/* The board as the whole computer: it broadcasts its own Wi-Fi hotspot,
   serves a tiny editor page, and runs submitted programs on-chip.
   Join "Aksa-XXXX", open http://192.168.4.1, type code, tap Run. */

#include <WebServer.h>
#include <WiFi.h>

extern "C" {
#include "vm.h"
}

#include "aksa_locale.h"
#include "index_html.h"

extern String device_output;
extern const AksaHost device_host;
void device_run_reset(unsigned long budget_ms);
bool device_run_timed_out();

static const unsigned long RUN_BUDGET_MS = 10000;
static const size_t MAX_SOURCE = 16 * 1024;

static WebServer server(80);
static AksaLocale loc_id, loc_en;

static void handleRun() {
    String src = server.arg("plain");
    if (src.length() == 0 || src.length() > MAX_SOURCE) {
        server.send(400, "text/plain; charset=utf-8", "bad request");
        return;
    }
    bool en = server.arg("locale") == "en";
    const AksaLocale *loc = en ? &loc_en : &loc_id;

    device_run_reset(RUN_BUDGET_MS);
    AksaErrors errs = {0};
    aksa_run(src.c_str(), loc, &errs, &device_host);

    if (errs.count > 0) {
        size_t cap = 64, len = 0;
        char *msgs = (char *)malloc(cap);
        msgs[0] = 0;
        aksa_errors_dump(loc, &errs, &msgs, &len, &cap);
        device_output += msgs;
        free(msgs);
    }
    if (device_run_timed_out())
        device_output += en ? "\n(stopped: over 10 seconds)\n"
                            : "\n(berhenti: lebih dari 10 detik)\n";

    server.send(200, "text/plain; charset=utf-8", device_output);
    Serial.printf("run: %d errors, free heap %u\n", errs.count, ESP.getFreeHeap());
}

void setup() {
    Serial.begin(115200);

    char err[128];
    if (aksa_locale_load(&loc_id, aksa_locale_id, err, sizeof err))
        Serial.printf("locale id: %s\n", err);
    if (aksa_locale_load(&loc_en, aksa_locale_en, err, sizeof err))
        Serial.printf("locale en: %s\n", err);

    char ssid[16];
    snprintf(ssid, sizeof ssid, "Aksa-%04X",
             (unsigned)(ESP.getEfuseMac() >> 32) & 0xFFFF);
    WiFi.softAP(ssid);

    server.on("/", HTTP_GET, [] { server.send_P(200, "text/html", AKSA_INDEX_HTML); });
    server.on("/run", HTTP_POST, handleRun);
    server.begin();
    Serial.printf("%s up at http://%s, free heap %u\n", ssid,
                  WiFi.softAPIP().toString().c_str(), ESP.getFreeHeap());
}

void loop() { server.handleClient(); }
