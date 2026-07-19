---
title: CLI
description: The aksa command line — run, check, and inspect programs.
---

The `aksa` binary is the native build of the same core that runs in the browser. Build it with `make` and run it from the repo root so it finds `locales/`.

## Commands

| Command                  | What it does                                              |
| ------------------------ | --------------------------------------------------------- |
| `aksa run file.aksa`     | Check the whole program, then run it                      |
| `aksa check file.aksa`   | List every mistake, without running                       |
| `aksa ast file.aksa`     | Print the parse tree                                      |
| `aksa tokens file.aksa`  | Print the token stream                                    |
| `aksa emit file.aksa`    | Emit the program as C source (the deploy path)            |

## Choosing a locale

Every command accepts `--locale` (default `en`):

```bash
./aksa run examples/hello.aksa
./aksa run examples/halo.aksa --locale id
```

The locale decides which keywords and builtin names the program is written in, and which language the error messages come out in.

## Examples

```bash
make                                  # build the CLI
./aksa check examples/hello.aksa      # every mistake at once, before running
./aksa emit examples/hello.aksa       # readable C, ready for a real board
make test                             # golden + differential test suites
```

The emitted C is proven by differential tests to behave exactly like the VM — what runs on the screen is what runs on the board.
