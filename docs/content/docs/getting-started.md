---
title: Getting Started
description: Run your first Aksa program in the browser, on the command line, or on a real board.
---

Aksa is a tiny programming language for kids. Keywords, builtins, and error messages all come from a locale dictionary, so the same language reads naturally in English, Indonesian, and any locale added later.

## In the browser

The easiest way to try Aksa is the browser editor — no installation at all. Open the editor, pick a lesson, press **Run**, and watch the robot draw or the console print.

```aksa
color("blue")
repeat 4 {
    forward(100)
    turn_right(90)
}
```

The editor checks your whole program as you type and lists every mistake in your language, with the line highlighted.

If you are building from source, `make web` builds the WebAssembly module and editor bundle, then serve the repo root and open `/web/`.

## On the command line

The core is pure C99 — build it with any C compiler:

```bash
make                                  # build the CLI
./aksa run examples/hello.aksa        # run a program (locale defaults to en)
./aksa run examples/halo.aksa --locale id
./aksa check file.aksa                # list every mistake, without running
```

Run `aksa` from the repo root so it finds `locales/`. See the [CLI reference](/reference/cli) for all commands.

## On a real board

An Aksa program can leave the screen: **Send to Device** in the editor turns your program into C, compiles it for an ESP32 board, and flashes it over USB — straight from the browser, nothing to install. The board is detected automatically (ESP32-C3 and C6 are supported today).

The [Aksabot kit](/aksabot/overview) will make this a complete hardware experience for kids.

## Next steps

- Follow the [lessons](/learn/lesson-01) — 15 graded steps from first words to a guessing game.
- Read the [language overview](/language/overview) for the full picture.
- Browse the [builtins](/reference/builtins) and [error codes](/reference/errors).
