#include "hal.h"
#include <stdio.h>

/* Desktop simulator HAL: mirrors the CLI headless board (core/main.c
   cli_host) so emitted programs match `aksa run` output — pin writes echo to
   stdout, reads are 0, waiting is skipped. */

void hal_pin_on(double pin) { printf("[pin %g ON]\n", pin); }
void hal_pin_off(double pin) { printf("[pin %g OFF]\n", pin); }
double hal_pin_read(double pin) { (void)pin; return 0; }
double hal_pin_read_analog(double pin) { (void)pin; return 0; }
void hal_wait(double ms) { (void)ms; /* no delay */ }
