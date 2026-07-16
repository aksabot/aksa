/* Board-side hooks for the interpreter: pin builtins go to the HAL, program
   output collects into a String the web handler returns, input reads as empty
   (the board has no stdin), and every run stops after a time budget so an
   endless loop can't wedge the board or trip the watchdog. */

#include <Arduino.h>

extern "C" {
#include "vm.h"
#include "hal.h"
}

String device_output;
static unsigned long deadline;
static bool timed_out;

void device_run_reset(unsigned long budget_ms) {
    device_output = "";
    deadline = millis() + budget_ms;
    timed_out = false;
}

bool device_run_timed_out() { return timed_out; }

static void dev_out(const char *text, void *) { device_output += text; }

static void dev_in(char *buf, int bufsz, void *) {
    if (bufsz > 0) buf[0] = 0;
}

static int dev_yield(void *) {
    if (millis() < deadline) return 0;
    timed_out = true;
    return 1;
}

static int dev_host(const char *canon, double num, const char *str,
                    double *result, void *) {
    (void)str;
    if (strcmp(canon, "pin_on") == 0) hal_pin_on(num);
    else if (strcmp(canon, "pin_off") == 0) hal_pin_off(num);
    else if (strcmp(canon, "pin_read") == 0) *result = hal_pin_read(num);
    else if (strcmp(canon, "pin_read_analog") == 0) *result = hal_pin_read_analog(num);
    else if (strcmp(canon, "wait") == 0) hal_wait(num);
    else return 0; /* turtle builtins: not available on the board */
    return 1;
}

/* extern: const alone would give it internal linkage in C++ */
extern const AksaHost device_host = {dev_out, dev_in, dev_yield, dev_host, nullptr};
