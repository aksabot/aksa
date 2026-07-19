---
title: Keywords
description: All 13 Aksa keywords, in English and Indonesian.
---

Aksa has exactly 13 keywords. They are never hardcoded — the lexer reads them from the locale dictionary, so every locale gets natural words for the same concepts.

| English    | Indonesian | Meaning                                          |
| ---------- | ---------- | ------------------------------------------------ |
| `if`       | `jika`     | Run a block only when a condition is true        |
| `else`     | `lainnya`  | Run another block when the condition is false    |
| `repeat`   | `ulangi`   | Repeat a block a fixed number of times           |
| `while`    | `selama`   | Repeat a block while a condition stays true      |
| `function` | `fungsi`   | Define your own command                          |
| `return`   | `kembali`  | Give an answer back from a function              |
| `true`     | `benar`    | The boolean value true                           |
| `false`    | `salah`    | The boolean value false                          |
| `and`      | `dan`      | Both sides must be true                          |
| `or`       | `atau`     | At least one side must be true                   |
| `not`      | `bukan`    | Flip true to false and back                      |
| `make`     | `buat`     | Create a variable (optional — `x = 1` works too) |
| `stop`     | `berhenti` | Break out of the current loop                    |

## Examples

```aksa
make age = 9               // 'make' is optional: age = 9 works too
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

Builtin commands like `print` and `forward` are not keywords — they come from the standard library table and are also localized. See [Builtins](/reference/builtins).
