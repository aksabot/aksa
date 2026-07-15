/* Device HAL for ESP32 (Arduino core). Compiled as C++ because Arduino.h is
   C++; the extern "C" block gives the definitions C linkage so C-compiled
   rt.c links against them.

   Pins are digital by default; read_analog uses the ADC. Numbers are doubles
   in v1 (PLAN §6.7) — cast at the HAL edge. */

#include <Arduino.h>

extern "C" {
#include "hal.h"
}

void hal_pin_on(double pin) {
    pinMode((int)pin, OUTPUT);
    digitalWrite((int)pin, HIGH);
}
void hal_pin_off(double pin) {
    pinMode((int)pin, OUTPUT);
    digitalWrite((int)pin, LOW);
}
double hal_pin_read(double pin) {
    pinMode((int)pin, INPUT);
    return digitalRead((int)pin);
}
double hal_pin_read_analog(double pin) { return analogRead((int)pin); }
void hal_wait(double ms) { delay((unsigned long)ms); }
