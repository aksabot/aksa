---
title: Aksabot
description: The upcoming Aksa hardware kit — a small robot board kids program in their own language.
---

> **Status: in development.** Aksabot is being prototyped now. Everything on this page describes the plan; details may change before launch.

Aksabot is a closed hardware kit built around the ESP32-C6, designed as the graduation step from the screen robot to a real machine — same language, same lessons, real lights and motors.

## No computer required

The board is its own coding environment. Power it on and it becomes a Wi-Fi hotspot serving the Aksa editor — join it from any phone, tablet, or laptop, write a program in the browser, and it runs right on the chip. No app, no installation, no internet.

## Components with names, not pin numbers

Kids shouldn't need to know that the LED sits on GPIO 4. Every Aksabot component has a name, and the board profile generates the wiring automatically as an Aksa preamble:

```aksa
buat lampu = 4
buat tombol = 9
```

So a program simply says:

```aksa
jika (baca(tombol) == 1) {
    nyalakan(lampu)
}
```

## What the kit adds

- Three new builtins for real actuators and sensors — PWM power, servo angles, and distance — see [Aksabot builtins](/aksabot/builtins).
- A hardware lesson track that parallels the 15 screen lessons: blink, button, servo wave, distance alarm.
- A text-based editor in v1; a block editor is on the roadmap.

## The road to production

1. Prototype from off-the-shelf modules.
2. Prove the software on the ESP32-C6.
3. Validate the curriculum with real kids.
4. Only then: custom PCB, casing, and production.

Follow progress on the [roadmap](/reference/roadmap).
