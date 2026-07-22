---
title: Peta Pin C6
description: Komponen bernama Aksabot dan pin GPIO-nya di ESP32-C6 Super Mini.
---

> **Status: direncanakan.** Peta ini untuk prototipe Aksabot di ESP32-C6 Super Mini. Detail bisa berubah sebelum PCB khusus dibuat.

> **Tier: Starter.** Peta ini khusus level dasar. Tingkat di atasnya bisa memakai pin atau komponen berbeda.

Setiap komponen Aksabot punya nama. Papan menghasilkan pembuka `buat nama = pin` otomatis (lihat [gambaran umum](/id/aksabot/overview)), jadi anak menulis `nyalakan(lampu)`, bukan `nyalakan(7)`.

## Komponen bernama → GPIO

| Komponen | Nama | GPIO | Perintah |
| --- | --- | --- | --- |
| Potensio | `potensio` | 0 | `baca_analog` |
| Joystick X | `joy_x` | 1 | `baca_analog` |
| Joystick Y | `joy_y` | 2 | `baca_analog` |
| LDR (cahaya) | `cahaya` | 3 | `baca_analog` |
| Suhu (LM35) | `suhu` | 4 | `baca_analog` |
| Joystick tekan | `joy_tombol` | 5 | `baca` |
| Mikrofon | `suara` | 6 | `baca` |
| LED | `lampu` | 7 | `nyalakan` / `matikan` / `atur` |
| LED warna (strip) | `lampu_warna` | 14 | `nyalakan` / `matikan` |
| LED bawaan | `lampu_papan` | 15 | `nyalakan` / `matikan` |
| Buzzer | `bel` | 18 | `nyalakan` / `matikan` |
| Tombol | `tombol` | 19 | `baca` |
| PIR (gerak) | `gerak` | 20 | `baca` |
| Servo | `servo` | 21 | `sudut` |
| Layar angka (TM1637) | `layar_clk` / `layar_data` | 22 / 23 | — |

**15 dari 15 pin usable terpakai.** Pin ADC (`baca_analog`) hanya GPIO 0–6.

## Pin yang dihindari

| Pin | Alasan |
| --- | --- |
| 8, 9 | Pin strapping / tombol BOOT — ganggu saat papan menyala |
| 12, 13 | Jalur USB — dipakai untuk flash & serial |
| 24–30 | Terhubung ke flash internal, tidak keluar ke header |

## Contoh (memakai nama)

```aksa
// Lampu malam: makin gelap, makin terang
selama benar {
    atur(lampu, 100 - baca_analog(cahaya))
    tunggu(100)
}
```
