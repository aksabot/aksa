---
title: CLI
description: Baris perintah aksa — menjalankan, memeriksa, dan membedah program.
---

Binari `aksa` adalah versi native dari inti yang sama dengan yang berjalan di browser. Bangun dengan `make` dan jalankan dari akar repo agar menemukan `locales/`.

## Perintah

| Perintah                 | Fungsinya                                                 |
| ------------------------ | --------------------------------------------------------- |
| `aksa run file.aksa`     | Periksa seluruh program, lalu jalankan                    |
| `aksa check file.aksa`   | Daftar semua kesalahan, tanpa menjalankan                 |
| `aksa ast file.aksa`     | Cetak pohon urai                                          |
| `aksa tokens file.aksa`  | Cetak aliran token                                        |
| `aksa emit file.aksa`    | Ubah program menjadi kode C (jalur kirim ke perangkat)    |

## Memilih bahasa

Setiap perintah menerima `--locale` (bawaan `en`):

```bash
./aksa run examples/hello.aksa
./aksa run examples/halo.aksa --locale id
```

Lokal menentukan kata kunci dan nama perintah yang dipakai program, serta bahasa pesan error-nya.

## Contoh

```bash
make                                  # bangun CLI
./aksa check examples/halo.aksa       # semua kesalahan sekaligus, sebelum berjalan
./aksa emit examples/halo.aksa        # C yang mudah dibaca, siap untuk papan
make test                             # suite tes golden + diferensial
```

Kode C hasil ubahan dibuktikan lewat tes diferensial agar berperilaku persis seperti VM — yang berjalan di layar sama dengan yang berjalan di papan.
