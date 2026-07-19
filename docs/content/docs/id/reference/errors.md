---
title: Kode Error
description: Setiap error Aksa, dengan pesannya dalam bahasa Indonesia dan Inggris.
---

Pesan error adalah produk sesungguhnya dari Aksa. Setiap kesalahan punya ID tetap, posisi di kode, dan penjelasan ramah anak dalam bahasa programnya. Seluruh program diperiksa sebelum dijalankan, jadi anak melihat semua kesalahannya sekaligus.

## Error pembacaan (E001–E005)

Ditemukan saat membaca dan mengurai kode.

| ID     | Indonesia                                                | Inggris                                              |
| ------ | -------------------------------------------------------- | ---------------------------------------------------- |
| `E001` | Ada tanda yang tidak dikenal: '{char}'                    | There is an unknown symbol: '{char}'                  |
| `E002` | Teks belum ditutup dengan tanda kutip "                   | This text is missing its closing quote "              |
| `E003` | Angka ini tidak bisa dibaca: '{text}'                     | This number can't be read: '{text}'                   |
| `E004` | Ada yang tidak kuduga di sini: '{text}'                   | I didn't expect this here: '{text}'                   |
| `E005` | Sepertinya ada '{text}' yang hilang                       | It looks like a '{text}' is missing                   |

## Error program (E100–E111)

Ditemukan pemeriksa sebelum program berjalan, atau oleh penjaga runtime saat berjalan.

| ID     | Indonesia                                                  | Inggris                                               |
| ------ | ----------------------------------------------------------- | ------------------------------------------------------ |
| `E100` | Kamu belum membuat '{name}'                                 | You haven't made '{name}' yet                          |
| `E101` | Tidak bisa membagi dengan nol                               | Can't divide by zero                                   |
| `E102` | Aku tidak kenal perintah '{name}'                           | I don't know the command '{name}'                      |
| `E103` | Jumlah isian untuk '{name}' tidak pas                       | The number of inputs for '{name}' doesn't match        |
| `E104` | Nilai-nilai tidak cocok untuk operasi ini                   | The values don't fit this operation                    |
| `E105` | Bagian di dalam ( ) harus bernilai benar atau salah         | The part inside ( ) must be true or false              |
| `E106` | '{name}' belum bisa dipakai di sini                         | '{name}' can't be used here yet                        |
| `E107` | 'berhenti' hanya boleh di dalam pengulangan                 | 'stop' only works inside a loop                        |
| `E108` | 'kembali' hanya boleh di dalam fungsi                       | 'return' only works inside a function                  |
| `E109` | Program ini terlalu dalam atau terlalu besar                | This program goes too deep or is too big               |
| `E110` | 'ulangi' butuh angka                                        | 'repeat' needs a number                                |
| `E111` | '{name}' sudah pernah dibuat                                | '{name}' has already been made                         |

Setiap pesan juga membawa nomor baris (`(baris {line})` / `(line {line})`), dan editor menyorot titik persisnya. Di papan sungguhan, error runtime seperti `E101` muncul lewat monitor serial dengan kata-kata ramah yang sama.
