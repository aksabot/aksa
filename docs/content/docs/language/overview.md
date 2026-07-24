---
title: Language Overview
description: What Aksa is, the ideas behind it, and what a program looks like.
---

Aksa (from *aksara* — script, letters; Sanskrit *akṣara*, "imperishable") is built on three ideas:

1. **Kids should code in their own language.** All 13 keywords, every builtin, and every error message come from a locale dictionary. Adding a language is adding one JSON file — nothing is hardcoded.
2. **Error messages are the real product.** Every mistake has a stable ID, a source position, and a kid-friendly localized explanation. The whole program is checked before it runs, so kids see *all* their mistakes at once — never a compiler stack trace.
3. **Zero installation, real graduation.** The core is pure C99: it runs natively, compiles to WebAssembly for the browser, and transpiles to C for flashing onto real ESP32 boards — the same program from screen robot to real LED.

## A first program

```aksa
// my first program
function greet(name) {
  return "hello " + name
}

repeat 3 {
  print(greet("world"))
}
```

The same program, same language, Indonesian locale:

```aksa
fungsi sapa(nama) {
  kembali "halo " + nama
}

ulangi 3 {
  cetak(sapa("dunia"))
}
```

## The essentials

- **Comments** start with `//` and run to the end of the line.
- **Identifiers** accept full Unicode, so kids name things in their own words.
- **Files** use the `.aksa` extension.
- **Types** are numbers, strings, and booleans — see [Types & Values](/language/types).
- Conditions must be boolean: `if (5)` is a friendly error, not a silent truthy guess.

## How it runs

```
source ──► lexer ──► parser ──► checker ──► bytecode VM   (learning mode)
        (locale-      AST     every kid-    └─► robot/sim in browser
         driven)              facing error
                              caught here    └─► C emitter ──► real board
                                                 (deploy mode)
```

The lexer maps localized keywords to abstract tokens, the checker catches every kid-facing error before anything runs, and the same checked program either runs in the bytecode VM (browser or CLI) or is emitted as C for a real board.
