# Aksa

**Bahasa pemrograman untuk anak-anak.** A tiny programming language for kids — with keywords in *their* language, Indonesian first.

```
# program pertamaku
fungsi sapa(nama) {
  kembali "halo " + nama
}

ulangi 3 {
  tulis(sapa("dunia"))
}
```

The same program, same language, English locale:

```
function greet(name) {
  return "hello " + name
}

repeat 3 {
  print(greet("world"))
}
```

Aksa (from *aksara* — script, letters; Sanskrit *akṣara*, "imperishable") is built on three ideas:

1. **Kids should code in their own language.** All 13 keywords, every builtin, and every error message come from a locale dictionary (`locales/id.json`, `locales/en.json`). Adding a language is adding one JSON file — nothing is hardcoded.
2. **Error messages are the real product.** Every mistake has a stable ID, a source position, and a kid-friendly localized explanation. The whole program is checked before it runs, so kids see *all* their mistakes at once — never a compiler stack trace.
3. **Zero installation, real graduation.** The core is pure C99 with no dependencies: it runs natively, compiles to WebAssembly for the browser, and (planned) transpiles to C for flashing onto real ESP32 boards — the same program from screen turtle to real LED.

## Status

Early days — the language core works, the kid-facing environment is next.

| Milestone | |
|---|---|
| Lexer, locale system (id/en, Unicode identifiers) | ✅ |
| Parser → AST, several errors per run | ✅ |
| Semantic checker (`aksa check`) | ✅ |
| Bytecode VM (`aksa run`: functions, loops, `tulis`/`tanya`) | ✅ |
| Browser editor, turtle graphics | ⏳ Phase 2 |
| Hardware simulator (LEDs, sensors) | ⏳ Phase 3 |
| Transpile to C, flash real ESP32 via WebSerial | ⏳ Phase 4–5 |

See `ROADMAP.md` for detail.

## Try it

Requires a C compiler and `make` (plus `emcc` for the browser demo).

```sh
make                                  # build the CLI
./aksa run examples/halo.aksa         # run a program (locale defaults to id)
./aksa run examples/hello.aksa --locale en
./aksa check file.aksa                # list every mistake, without running
./aksa ast file.aksa                  # peek at the parse tree
make test                             # run the test suites

make wasm                             # build the WebAssembly module
python3 -m http.server                # then open http://localhost:8000/web/
```

Run `aksa` from the repo root so it finds `locales/`.

## The language (v1)

```
buat umur = 9              # 'buat' is optional: umur = 9 works too
jika (umur > 7) {
  tulis("sudah besar")
} lainnya {
  tulis("masih kecil")
}

i = 3
selama (i > 0) {
  tulis(i)
  i = i - 1
  jika (i == 1) { berhenti }
}
```

- **Types:** `angka` (numbers), `teks` (strings, `+` concatenates), `logika` (`benar`/`salah`). Conditions must be `logika` — `jika (5)` is an error with a friendly explanation, not a silent truthy guess.
- **Keywords (id / en):** jika/if, lainnya/else, ulangi/repeat, selama/while, fungsi/function, kembali/return, benar/true, salah/false, dan/and, atau/or, bukan/not, buat/make, berhenti/stop.
- **Builtins:** console (`tulis`, `tanya`), turtle (`maju`, `belok_kanan`, …) and hardware (`nyalakan`, `baca`, `tunggu`, …) — the latter two groups arrive with the browser environment and simulator.
- Identifiers accept full Unicode, so kids name things in their own words. Comments start with `#`.

## How it works

```
source ──► lexer ──► parser ──► checker ──► bytecode VM   (learning mode)
        (locale-      AST     every kid-    └─► turtle/sim in browser
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
wasm/      Emscripten glue        web/  bare browser demo
```
