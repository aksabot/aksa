---
title: Roadmap
description: What's done, what's next, and where Aksa is headed.
---

- [x] P0: scaffold, Makefile (native+wasm), test harness, CLI `aksa tokens`
- [x] P1a: locale loader + lexer (id/en, Unicode idents, E001–E003) + wasm demo page
- [x] P1b: parser → AST (recursive descent, panic-mode recovery, `aksa ast`)
- [x] P1c: semantic checker (`aksa check`; static E100–E111, runs before every `aksa run`)
- [x] P1d: bytecode VM, `aksa run` (tulis/tanya, functions, loops, localized runtime errors)
- [x] P1e: golden tests (40 ok + 36 error programs, `tests/golden.sh` in `make test`)
- [x] P2a: browser run (console via WASM hooks, inline async `tanya`, stop button, instruction-budget yield)
- [x] P2b: CodeMirror 6 editor (bun-bundled; locale-driven highlighting, live squiggles, error panel)
- [x] P2c: turtle canvas (animated draw, live speed slider, reset, id color names)
- [x] P3: hardware simulator (virtual board: 3 LEDs, button, slider, buzzer, fan; pin builtins + non-blocking tunggu; examples dropdown; headless CLI board for 10 golden tests)
- [x] P4: C emitter (`aksa emit`) + tiny value runtime + sim/esp32 HAL; differential tests (`tests/diff.sh`) prove emitted C compiles `-Werror` and matches the VM on all golden ok/err programs (E109 depth excluded)
- [x] P5: compile server (Bun + arduino-cli in Docker, ESP32) + browser flasher (esptool-js) + serial monitor; flash path untested on a real board
- [ ] P5 deferred: test on real ESP32; `tanya` on device (serial read in HAL); Pico/UF2; per-IP rate limit; build+test Docker image (no Docker locally); flash.js: call requestPort() before compile (fail fast if no board + avoid user-gesture timeout)
- [x] P6g: reference-counted heap strings in emitted C (rt.c); loops that build text free per-iteration, no leak on device; tests/mem.sh (AK_LIVE=0) + ASan-clean corpus
- [x] P6a (language only): IDE split into language page (lessons+turtle) and device page (board+flash); 15 graded lessons (id+en), basic→middle→advanced; share via URL hash
- [x] P6c: drawing cursor renamed turtle→robot, arrow replaced with a robot character (rounded body, eyes, nose showing heading)
- [x] P6d: lessons split into one file per locale (lessons.id.js/lessons.en.js), lessons.js just assembles the map
- [x] P6e: all JS → strict TypeScript (web+server, bun-native); tsconfig + `make typecheck`
- [x] P6f: auto-detect chip on flash (esptool-js CHIP_NAME → board); compile targets esp32c3/esp32c6, bootloader at 0x0; others rejected
- [ ] P6b: more locales; device lesson track after real-board testing; per-board GPIO pin map (C3 vs C6). Deliverable: Aksabot C6 pin map (named components → GPIO)
- [ ] P7: on-device runtime — ESP32-C3 is its own hotspot + editor + interpreter (`make device` / `device-flash`); compiles at 78% flash; check off after real-board test (LED polarity, heap, both locales)
- [ ] P8: Aksabot (closed kit, ESP32-C6) — reuse P7 AP+editor+on-chip VM; board profile (named components via generated `buat name = gpio` preamble); 3 new snake_case builtins (`atur`/pin_pwm, `sudut`/servo_write, `baca_jarak`/pin_read_distance) + HAL (esp32/sim); hardware lesson track (parallel to 15 screen lessons). v1 editor = text
- [ ] Production order (Aksabot): 1 prototype from off-the-shelf modules → prove C6 software → prove curriculum with kids → PCB+casing → mass. No PCB before wiring+curriculum validated
- [ ] Block+text editor: drag-drop blocks that emit Aksa text side-by-side. Deferred until text editor v1 proves out
- [x] Docs site (docs.aksabot.com): static bosia app in `docs/` — en + /id, language guide, builtins/errors/CLI reference, 15 lessons, Aksabot section, search, llms.txt, Cloudflare deploy
