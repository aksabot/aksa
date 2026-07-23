---
title: Builtins
description: Every built-in command, in English and Indonesian.
---

Builtins are the commands Aksa ships with. Like keywords, their names come from the locale dictionary — all names are lowercase `snake_case` in every locale. There are 14 today, in three groups.

## Console

| English    | Indonesian | What it does                                        |
| ---------- | ---------- | --------------------------------------------------- |
| `print(…)` | `tulis(…)` | Show values on the screen (any number of inputs)    |
| `ask(…)`   | `tanya(…)` | Ask a question and wait for the answer              |

## Robot (drawing)

The robot draws while it walks across the canvas.

| English         | Indonesian      | What it does                            |
| --------------- | --------------- | --------------------------------------- |
| `forward(n)`    | `maju(n)`       | Walk forward `n` steps, drawing a line  |
| `backward(n)`   | `mundur(n)`     | Walk backward `n` steps                 |
| `turn_right(d)` | `belok_kanan(d)`| Turn right by `d` degrees               |
| `turn_left(d)`  | `belok_kiri(d)` | Turn left by `d` degrees                |
| `color(name)`   | `warna(nama)`   | Change the pen color                    |
| `pen_up()`      | `angkat_pena()` | Lift the pen — walk without drawing     |
| `pen_down()`    | `turunkan_pena()`| Put the pen back down                  |

## Hardware

These work against the virtual board in the browser and against real pins on a device.

| English            | Indonesian        | What it does                                  |
| ------------------ | ----------------- | --------------------------------------------- |
| `turn_on(pin)`     | `nyalakan(pin)`   | Turn a pin on (light an LED)                  |
| `turn_off(pin)`    | `matikan(pin)`    | Turn a pin off                                |
| `read(pin)`        | `baca(pin)`       | Read a digital pin (button pressed or not)    |
| `read_analog(pin)` | `baca_analog(pin)`| Read an analog pin (slider, sensor)           |
| `wait(sec)`        | `tunggu(detik)`   | Pause for `sec` seconds; decimals allowed (`0.1` = 100ms) |

## Example

```aksa
if (read_analog(2) > 30) {
    turn_on(5)
} else {
    turn_off(5)
}
```

Calling a builtin with the wrong number of inputs is error `E103`, and a name Aksa doesn't know is `E102` — both caught before the program runs.

Three more hardware builtins are planned for the [Aksabot kit](/aksabot/builtins).
