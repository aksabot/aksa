---
title: Aksabot Builtins
description: The three planned hardware builtins that arrive with the Aksabot kit.
---

> **Status: planned.** These builtins are part of the Aksabot kit and are not in the language yet. Names follow the same lowercase `snake_case` rule as every other builtin; the final English names will be settled with the kit.

Aksabot adds three commands to the [existing hardware group](/reference/builtins), covering the kit's actuators and sensor:

| Indonesian        | Operation           | What it does                                        |
| ----------------- | ------------------- | --------------------------------------------------- |
| `atur(pin, n)`    | PWM write           | Set a pin's power from 0–100 — dim a light, set a motor speed |
| `sudut(pin, d)`   | Servo write         | Turn a servo to an angle in degrees                 |
| `baca_jarak(pin)` | Distance read       | Measure distance with the ultrasonic sensor         |

## What they will look like

A night light that dims as the room gets brighter:

```aksa
selama benar {
    buat terang = baca_analog(sensor)
    atur(lampu, 100 - terang)
    tunggu(1)
}
```

A radar that waves the servo and sounds an alarm when something is near:

```aksa
selama benar {
    sudut(servo, 45)
    jika (baca_jarak(sonar) < 10) {
        nyalakan(alarm)
    }
    tunggu(1)
}
```

Both run in the simulator first, then on the real kit — like every other Aksa program.
