# Roadmap (detail in PLAN.md)

- [x] P0: scaffold, Makefile (native+wasm), test harness, CLI `aksa tokens`
- [x] P1a: locale loader + lexer (id/en, Unicode idents, E001–E003) + wasm demo page
- [x] P1b: parser → AST (recursive descent, panic-mode recovery, `aksa ast`)
- [x] P1c: semantic checker (`aksa check`; static E100–E111, runs before every `aksa run`)
- [x] P1d: bytecode VM, `aksa run` (tulis/tanya, functions, loops, localized runtime errors)
- [x] P1e: golden tests (40 ok + 36 error programs, `tests/golden.sh` in `make test`)
- [ ] P2: browser editor, turtle, run controls
- [ ] P3: hardware simulator
- [ ] P4: C emitter + differential tests
- [ ] P5: compile server + WebSerial flash
- [ ] P6: lessons, sharing, more locales
