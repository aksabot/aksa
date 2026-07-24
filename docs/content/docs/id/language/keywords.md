---
title: Kata Kunci
description: Semua 13 kata kunci Aksa, dalam bahasa Indonesia dan Inggris.
---

Aksa punya tepat 13 kata kunci. Tidak ada yang tertanam di kode — lexer membacanya dari kamus bahasa, jadi setiap lokal mendapat kata yang alami untuk konsep yang sama.

| Indonesia  | Inggris    | Arti                                              |
| ---------- | ---------- | ------------------------------------------------- |
| `jika`     | `if`       | Jalankan blok hanya saat syaratnya benar          |
| `lainnya`  | `else`     | Jalankan blok lain saat syaratnya salah           |
| `ulangi`   | `repeat`   | Ulangi blok sebanyak angka tertentu               |
| `selama`   | `while`    | Ulangi blok selama syaratnya masih benar          |
| `fungsi`   | `function` | Buat perintahmu sendiri                           |
| `kembali`  | `return`   | Kembalikan jawaban dari fungsi                    |
| `benar`    | `true`     | Nilai logika benar                                |
| `salah`    | `false`    | Nilai logika salah                                |
| `dan`      | `and`      | Kedua sisi harus benar                            |
| `atau`     | `or`       | Salah satu sisi harus benar                       |
| `bukan`    | `not`      | Balikkan benar menjadi salah dan sebaliknya       |
| `buat`     | `make`     | Buat variabel (opsional — `x = 1` juga bisa)      |
| `berhenti` | `stop`     | Keluar dari pengulangan saat itu juga             |

## Contoh

```aksa
buat umur = 9              // 'buat' opsional: umur = 9 juga bisa
jika (umur > 7) {
  cetak("anak besar")
} lainnya {
  cetak("anak kecil")
}

i = 3
selama (i > 0) {
  cetak(i)
  i = i - 1
  jika (i == 1) { berhenti }
}
```

Perintah bawaan seperti `cetak` dan `maju` bukan kata kunci — mereka berasal dari tabel pustaka standar dan juga dilokalkan. Lihat [Perintah Bawaan](/id/reference/builtins).
