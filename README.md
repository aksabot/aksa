# Aksa

A tiny programming language for kids — with keywords in *their* language.

```
// my first program
function greet(name) {
  return "hello " + name
}

repeat 3 {
  print(greet("world"))
}
```

The same program, same language, Indonesian locale:

```
fungsi sapa(nama) {
  kembali "halo " + nama
}

ulangi 3 {
  tulis(sapa("dunia"))
}
```

Aksa (from *aksara* — script, letters; Sanskrit *akṣara*, "imperishable") is built on three ideas:

1. **Kids should code in their own language.** All 13 keywords, every builtin, and every error message come from a locale dictionary (`locales/en.json`, `locales/id.json`). Adding a language is adding one JSON file — nothing is hardcoded.
2. **Error messages are the real product.** Every mistake has a stable ID, a source position, and a kid-friendly localized explanation. The whole program is checked before it runs, so kids see *all* their mistakes at once — never a compiler stack trace.
3. **Zero installation, real graduation.** The core is pure C99 with no dependencies: it runs natively, compiles to WebAssembly for the browser, and (planned) transpiles to C for flashing onto real ESP32 boards — the same program from screen robot to real LED.

## Status

Early days — the language core works, the kid-facing environment is next.

| Milestone | |
|---|---|
| Lexer, locale system (en/id, Unicode identifiers) | ✅ |
| Parser → AST, several errors per run | ✅ |
| Semantic checker (`aksa check`) | ✅ |
| Bytecode VM (`aksa run`: functions, loops, `tulis`/`tanya`) | ✅ |
| Browser editor, robot graphics, run controls | ✅ |
| Hardware simulator (LEDs, sensors) | ⏳ Phase 3 |
| Transpile to C, flash real ESP32 via WebSerial | ⏳ Phase 4–5 |

See `ROADMAP.md` for detail.

## Try it

Requires a C compiler and `make` (plus `emcc` and `bun` for the browser editor).

```sh
make                                  # build the CLI
./aksa run examples/hello.aksa        # run a program (locale defaults to en)
./aksa run examples/halo.aksa --locale id
./aksa check file.aksa                # list every mistake, without running
./aksa ast file.aksa                  # peek at the parse tree
make test                             # run the test suites

make play                             # build the WASM module + editor bundle
python3 -m http.server                # then open http://localhost:8000/play/
```

Run `aksa` from the repo root so it finds `locales/`.

## The language (v1)

```
make age = 9               # 'make' is optional: age = 9 works too
if (age > 7) {
  print("big kid")
} else {
  print("little kid")
}

i = 3
while (i > 0) {
  print(i)
  i = i - 1
  if (i == 1) { stop }
}
```

- **Types:** numbers, strings (`+` concatenates), booleans (`true`/`false`). Conditions must be boolean — `if (5)` is an error with a friendly explanation, not a silent truthy guess.
- **Keywords (en / id):** if/jika, else/lainnya, repeat/ulangi, while/selama, function/fungsi, return/kembali, true/benar, false/salah, and/dan, or/atau, not/bukan, make/buat, stop/berhenti.
- **Builtins:** console (`print`, `ask`), robot in the browser (`forward`, `turn_right`, `color`, …), and hardware (`turn_on`, `read`, `wait`, …) — the hardware group arrives with the simulator.
- Identifiers accept full Unicode, so kids name things in their own words. Comments start with `//`.

## How it works

```
source ──► lexer ──► parser ──► checker ──► bytecode VM   (learning mode)
        (locale-      AST     every kid-    └─► robot/sim in browser
         driven)              facing error
                              caught here    └─► C emitter ──► real board
                                                 (deploy mode, planned)
```

Everything under `core/` is C99 with zero dependencies — the exact same code runs as a native CLI and as WASM in the browser. `PLAN.md` has the full design.

## Repo layout

```
core/      lexer, parser, checker, VM — pure C99
locales/   id.json, en.json — keywords, builtins, error messages
tests/     one small suite per component (make test)
examples/  graded example programs
wasm/      Emscripten glue        play/ browser editor (CodeMirror + robot)
```
