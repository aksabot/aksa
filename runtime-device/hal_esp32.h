#ifndef AKSA_HAL_ESP32_H
#define AKSA_HAL_ESP32_H

/* Device HAL for ESP32 (Arduino core). Not built or tested in P4 — the
   compile server and WebSerial flashing that exercise it arrive in P5. It
   exists now so the emitter's HAL boundary is real: swap hal_sim.c for this
   translation unit and the same emitted C runs on hardware.

   Pins are digital by default; read_analog uses the ADC. Numbers are doubles
   in v1 (PLAN §6.7) — cast at the HAL edge. */

#include <Arduino.h>

static inline void hal_pin_on(double pin) {
    pinMode((int)pin, OUTPUT);
    digitalWrite((int)pin, HIGH);
}
static inline void hal_pin_off(double pin) {
    pinMode((int)pin, OUTPUT);
    digitalWrite((int)pin, LOW);
}
static inline double hal_pin_read(double pin) {
    pinMode((int)pin, INPUT);
    return digitalRead((int)pin);
}
static inline double hal_pin_read_analog(double pin) {
    return analogRead((int)pin);
}
static inline void hal_wait(double ms) { delay((unsigned long)ms); }

#endif
