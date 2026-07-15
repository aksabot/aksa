#ifndef AKSA_HAL_H
#define AKSA_HAL_H

/* Hardware abstraction for emitted programs. Desktop tests link hal_sim.c;
   real devices link a board HAL (hal_esp32.h) instead. The runtime (core/rt.c)
   only ever calls through this interface. */

void hal_pin_on(double pin);
void hal_pin_off(double pin);
double hal_pin_read(double pin);
double hal_pin_read_analog(double pin);
void hal_wait(double ms);

#endif
