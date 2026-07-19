---
title: Alur Kendali
description: Memilih dengan jika/lainnya, mengulang dengan ulangi dan selama, dan menghentikan pengulangan.
---

## Memilih: `jika` / `lainnya`

Bagian di dalam `( )` harus bernilai benar atau salah. Saat benar, blok pertama berjalan; kalau tidak, blok `lainnya`:

```aksa
umur = tanya("Berapa umurmu?")
jika (umur >= 10) {
    tulis("Wah, kamu sudah besar!")
} lainnya {
    tulis("Kamu masih kecil, tapi hebat!")
}
```

Rangkaian pilihan juga bisa:

```aksa
jika (tebakan < rahasia) {
    tulis("Terlalu kecil!")
} lainnya jika (tebakan > rahasia) {
    tulis("Terlalu besar!")
} lainnya {
    tulis("Tepat!")
}
```

## Mengulang sebanyak angka: `ulangi`

`ulangi` menerima angka dan menjalankan bloknya sebanyak itu:

```aksa
ulangi 4 {
    maju(100)
    belok_kanan(90)
}
```

Memberi `ulangi` sesuatu yang bukan angka adalah error `E110`.

## Mengulang selama benar: `selama`

`selama` terus berjalan selama syaratnya benar:

```aksa
buat angka = 5
selama (angka > 0) {
    tulis(angka)
    angka = angka - 1
}
tulis("Meluncur!")
```

Di browser, pengulangan tanpa akhir tidak akan pernah membekukan halaman — runtime menyerahkan giliran secara berkala dan tombol Stop selalu bekerja.

## Keluar dari pengulangan: `berhenti`

`berhenti` keluar dari pengulangan saat itu juga:

```aksa
i = 3
selama (i > 0) {
    tulis(i)
    i = i - 1
    jika (i == 1) { berhenti }
}
```

Memakai `berhenti` di luar pengulangan adalah error `E107`.
