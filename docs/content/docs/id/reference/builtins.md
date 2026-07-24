---
title: Perintah Bawaan
description: Semua perintah bawaan, dalam bahasa Indonesia dan Inggris.
---

Perintah bawaan adalah perintah yang sudah disertakan Aksa. Seperti kata kunci, namanya berasal dari kamus bahasa — semua nama memakai `snake_case` huruf kecil di setiap lokal. Saat ini ada 14, dalam tiga kelompok.

## Konsol

| Indonesia  | Inggris    | Fungsinya                                         |
| ---------- | ---------- | ------------------------------------------------- |
| `cetak(…)` | `print(…)` | Tampilkan nilai di layar (isian berapa pun)       |
| `tanya(…)` | `ask(…)`   | Ajukan pertanyaan dan tunggu jawabannya           |

## Robot (menggambar)

Robot menggambar sambil berjalan di kanvas.

| Indonesia         | Inggris         | Fungsinya                                |
| ----------------- | --------------- | ---------------------------------------- |
| `maju(n)`         | `forward(n)`    | Maju `n` langkah sambil menggambar garis |
| `mundur(n)`       | `backward(n)`   | Mundur `n` langkah                       |
| `belok_kanan(d)`  | `turn_right(d)` | Belok kanan `d` derajat                  |
| `belok_kiri(d)`   | `turn_left(d)`  | Belok kiri `d` derajat                   |
| `warna(nama)`     | `color(name)`   | Ganti warna pena                         |
| `angkat_pena()`   | `pen_up()`      | Angkat pena — berjalan tanpa menggambar  |
| `turunkan_pena()` | `pen_down()`    | Turunkan pena kembali                    |

## Perangkat keras

Perintah ini bekerja pada papan virtual di browser dan pada pin sungguhan di perangkat.

| Indonesia          | Inggris            | Fungsinya                                        |
| ------------------ | ------------------ | ------------------------------------------------ |
| `nyalakan(pin)`    | `turn_on(pin)`     | Nyalakan pin (hidupkan LED)                      |
| `matikan(pin)`     | `turn_off(pin)`    | Matikan pin                                      |
| `baca(pin)`        | `read(pin)`        | Baca pin digital (tombol ditekan atau tidak)     |
| `baca_analog(pin)` | `read_analog(pin)` | Baca pin analog (slider, sensor)                 |
| `tunggu(detik)`    | `wait(sec)`        | Jeda `detik` detik; boleh desimal (`0.1` = 100md) |

## Contoh

```aksa
jika (baca_analog(2) > 30) {
    nyalakan(5)
} lainnya {
    matikan(5)
}
```

Memanggil perintah dengan jumlah isian yang salah adalah error `E103`, dan nama yang tidak dikenal Aksa adalah `E102` — keduanya ditangkap sebelum program berjalan.

Tiga perintah perangkat keras lagi direncanakan untuk [kit Aksabot](/id/aksabot/builtins).
