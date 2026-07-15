# Roadmap (detail in PLAN.md)

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
- [ ] P5 deferred: test on real ESP32; `tanya` on device (serial read in HAL); free/reuse strings in rt.c loops; Pico/UF2; per-IP rate limit; build+test Docker image (no Docker locally); flash.js: call requestPort() before compile (fail fast if no board + avoid user-gesture timeout)
- [x] P6a (language only): IDE split into language page (lessons+turtle) and device page (board+flash); 9 graded lessons (id+en); share via URL hash
- [ ] P6b: more locales; device lesson track after real-board testing
