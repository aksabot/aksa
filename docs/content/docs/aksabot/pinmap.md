---
title: C6 Pin Map
description: Aksabot's named components and their GPIO pins on the ESP32-C6 Super Mini.
---

> **Status: planned.** This map targets the Aksabot prototype on the ESP32-C6 Super Mini. Details may change before the custom PCB.

> **Tier: Starter.** This map is the basic level only. Higher tiers may use different pins or components.

Every Aksabot component has a name. The board generates a `make name = pin` preamble automatically (see the [overview](/aksabot/overview)), so a child writes `turn_on(light)`, not `turn_on(7)`.

## Named components → GPIO

| Component | Name | GPIO | Command |
| --- | --- | --- | --- |
| Potentiometer | `potensio` | 0 | `read_analog` |
| Joystick X | `joy_x` | 1 | `read_analog` |
| Joystick Y | `joy_y` | 2 | `read_analog` |
| LDR (light) | `cahaya` | 3 | `read_analog` |
| Temperature (LM35) | `suhu` | 4 | `read_analog` |
| Joystick press | `joy_tombol` | 5 | `read` |
| Microphone | `suara` | 6 | `read` |
| LED | `lampu` | 7 | `turn_on` / `turn_off` / `set` |
| Color LED (strip) | `lampu_warna` | 14 | `turn_on` / `turn_off` |
| Onboard LED | `lampu_papan` | 15 | `turn_on` / `turn_off` |
| Buzzer | `bel` | 18 | `turn_on` / `turn_off` |
| Button | `tombol` | 19 | `read` |
| PIR (motion) | `gerak` | 20 | `read` |
| Servo | `servo` | 21 | `angle` |
| Numeric display (TM1637) | `layar_clk` / `layar_data` | 22 / 23 | — |

**15 of 15 usable pins are taken.** ADC pins (`read_analog`) are GPIO 0–6 only.

## Battery sense (standard)

Boards powered by an 18650 read the cell voltage on **GPIO6 / ADC1_6** through a `B+ → 100k → GPIO6 → 100k → GND` divider (plus a 100nF cap to GND). The firmware serves the level at `GET /bat` (plain 0–100). The board's "BAT" LED only shows *charging* and can't be read by code.

> **Starter exception:** the Starter tier uses GPIO6 for the microphone (`suara`) and has no spare ADC, so it has **no battery sense**. Only boards with a free ADC pin reserve GPIO6 for battery.

## Pins to avoid

| Pin | Reason |
| --- | --- |
| 8, 9 | Strapping pins / BOOT button — interfere at power-up |
| 12, 13 | USB lines — used for flashing & serial |
| 24–30 | Wired to internal flash, not broken out to headers |

## Example (using names)

```aksa
// Night light: darker room, brighter lamp
while true {
    set(lampu, 100 - read_analog(cahaya))
    wait(1)
}
```

> Component *names* stay the same across locales — they are variables the board declares, not keywords.
