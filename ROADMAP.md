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
- [ ] P4: C emitter + differential tests
- [ ] P5: compile server + WebSerial flash
- [ ] P6: lessons, sharing, more locales
