/* Board-side hooks for the interpreter. The program runs in its own task
   (aksaTask) so the web server stays responsive; its printed text flows to the
   browser through a FreeRTOS stream buffer — single-writer (the program),
   single-reader (the /output handler), so it's lock-free and can't corrupt
   like a shared String would. A run ends only when the page asks it to (Stop
   or a new Run), which lets `selama (benar)` interactive programs run forever.
   Pin builtins go to the HAL; input reads as empty (no stdin on the board). */

#include <Arduino.h>

extern "C" {
#include "hal.h"
#include "vm.h"
}

static StreamBufferHandle_t out_buf; /* program → web; SPSC, no lock needed */
static volatile bool stop_flag;

/* Called on loopTask before starting a run, once we know no program runs. */
void device_out_begin() {
  if (!out_buf)
    out_buf = xStreamBufferCreate(2048, 1);
  xStreamBufferReset(out_buf);
  stop_flag = false;
}

void device_request_stop() { stop_flag = true; }

/* Push text into the buffer. 2 KB cap; if the page polls too slowly
   the overflow is dropped rather than blocking the program — fine for the
   low-volume "pushed/released" case. Grow the buffer if a program floods it. */
void device_out_write(const char *t) {
  if (out_buf)
    xStreamBufferSend(out_buf, t, strlen(t), 0);
}

/* Web side drains whatever text is waiting. */
size_t device_out_drain(char *buf, size_t n) {
  return out_buf ? xStreamBufferReceive(out_buf, buf, n, 0) : 0;
}

static void dev_out(const char *text, void *) { device_out_write(text); }

static void dev_in(char *buf, int bufsz, void *) {
  if (bufsz > 0)
    buf[0] = 0;
}

static int dev_yield(void *) {
  delay(0);                 /* feed the watchdog + let the radio/idle run */
  return stop_flag ? 1 : 0; /* stop only on request — forever-loops allowed */
}

/* Runs once per loop iteration, so Stop works even when the loop spends its time
   sleeping in `tunggu`. Also throttles hot forever-loops (a `selama (benar)` with
   no `tunggu`): shed ~1 ms of real sleep per THROTTLE_MS of spinning so the board
   stays cool. A loop that already waits, or finishes within THROTTLE_MS, is
   untouched. Lower THROTTLE_MS = cooler + slower; tune on hardware. */
static const unsigned long THROTTLE_MS = 10;
static int dev_poll(void *) {
  static unsigned long last;
  unsigned long now = millis();
  if (now - last >= THROTTLE_MS) {
    delay(1);
    last = now;
  }
  return stop_flag;
}

/* Sleep in small chunks so Stop is noticed within ~5 ms instead of at the end
   of a long wait; delay() also feeds the watchdog. */
static void dev_wait(double ms) {
  unsigned long start = millis();
  while (millis() - start < (unsigned long)ms && !stop_flag)
    delay(5);
}

static int dev_host(const char *canon, double num, const char *str,
                    double *result, void *) {
  (void)str;
  if (strcmp(canon, "pin_on") == 0)
    hal_pin_on(num);
  else if (strcmp(canon, "pin_off") == 0)
    hal_pin_off(num);
  else if (strcmp(canon, "pin_read") == 0)
    *result = hal_pin_read(num);
  else if (strcmp(canon, "pin_read_analog") == 0)
    *result = hal_pin_read_analog(num);
  else if (strcmp(canon, "wait") == 0)
    dev_wait(num * 1000.0); /* tunggu takes seconds; dev_wait sleeps ms */
  else
    return 0; /* turtle builtins: not available on the board */
  return 1;
}

/* extern: const alone would give it internal linkage in C++ */
extern const AksaHost device_host = {.out = dev_out,
                                     .in = dev_in,
                                     .yield = dev_yield,
                                     .host = dev_host,
                                     .user = nullptr,
                                     .poll = dev_poll};
