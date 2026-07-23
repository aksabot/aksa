---
title: Perintah Aksabot
description: Tiga perintah perangkat keras yang direncanakan hadir bersama kit Aksabot.
---

> **Status: direncanakan.** Perintah-perintah ini bagian dari kit Aksabot dan belum ada di bahasa. Namanya mengikuti aturan `snake_case` huruf kecil seperti perintah bawaan lain; nama Inggris finalnya akan ditetapkan bersama kit.

Aksabot menambah tiga perintah ke [kelompok perangkat keras yang ada](/id/reference/builtins), untuk penggerak dan sensor kit:

| Indonesia         | Operasi        | Fungsinya                                                     |
| ----------------- | -------------- | ------------------------------------------------------------- |
| `atur(pin, n)`    | Tulis PWM      | Atur daya pin dari 0–100 — redupkan lampu, atur kecepatan motor |
| `sudut(pin, d)`   | Tulis servo    | Putar servo ke sudut dalam derajat                            |
| `baca_jarak(pin)` | Baca jarak     | Ukur jarak dengan sensor ultrasonik                           |

## Seperti apa nantinya

Lampu tidur yang meredup saat ruangan makin terang:

```aksa
selama benar {
    buat terang = baca_analog(sensor)
    atur(lampu, 100 - terang)
    tunggu(1)
}
```

Radar yang melambaikan servo dan membunyikan alarm saat ada yang dekat:

```aksa
selama benar {
    sudut(servo, 45)
    jika (baca_jarak(sonar) < 10) {
        nyalakan(alarm)
    }
    tunggu(1)
}
```

Keduanya berjalan di simulator dulu, lalu di kit sungguhan — seperti setiap program Aksa lainnya.
