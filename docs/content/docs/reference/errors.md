---
title: Error Codes
description: Every Aksa error, with its message in English and Indonesian.
---

Error messages are Aksa's real product. Every mistake has a stable ID, a source position, and a kid-friendly explanation in the program's locale. The whole program is checked before it runs, so kids see all their mistakes at once.

## Reading errors (E001–E005)

Found while reading and parsing the code.

| ID     | English                                              | Indonesian                                               |
| ------ | ---------------------------------------------------- | -------------------------------------------------------- |
| `E001` | There is an unknown symbol: '{char}'                  | Ada tanda yang tidak dikenal: '{char}'                    |
| `E002` | This text is missing its closing quote "              | Teks belum ditutup dengan tanda kutip "                   |
| `E003` | This number can't be read: '{text}'                   | Angka ini tidak bisa dibaca: '{text}'                     |
| `E004` | I didn't expect this here: '{text}'                   | Ada yang tidak kuduga di sini: '{text}'                   |
| `E005` | It looks like a '{text}' is missing                   | Sepertinya ada '{text}' yang hilang                       |

## Program errors (E100–E111)

Found by the checker before the program runs, or by runtime guards while it runs.

| ID     | English                                               | Indonesian                                                 |
| ------ | ----------------------------------------------------- | ---------------------------------------------------------- |
| `E100` | You haven't made '{name}' yet                          | Kamu belum membuat '{name}'                                 |
| `E101` | Can't divide by zero                                   | Tidak bisa membagi dengan nol                               |
| `E102` | I don't know the command '{name}'                      | Aku tidak kenal perintah '{name}'                           |
| `E103` | The number of inputs for '{name}' doesn't match        | Jumlah isian untuk '{name}' tidak pas                       |
| `E104` | The values don't fit this operation                    | Nilai-nilai tidak cocok untuk operasi ini                   |
| `E105` | The part inside ( ) must be true or false              | Bagian di dalam ( ) harus bernilai benar atau salah         |
| `E106` | '{name}' can't be used here yet                        | '{name}' belum bisa dipakai di sini                         |
| `E107` | 'stop' only works inside a loop                        | 'berhenti' hanya boleh di dalam pengulangan                 |
| `E108` | 'return' only works inside a function                  | 'kembali' hanya boleh di dalam fungsi                       |
| `E109` | This program goes too deep or is too big               | Program ini terlalu dalam atau terlalu besar                |
| `E110` | 'repeat' needs a number                                | 'ulangi' butuh angka                                        |
| `E111` | '{name}' has already been made                         | '{name}' sudah pernah dibuat                                |

Every message also carries the line number (`(line {line})` / `(baris {line})`), and the editor highlights the exact spot. On a real board, runtime errors like `E101` appear over the serial monitor in the same friendly words.
