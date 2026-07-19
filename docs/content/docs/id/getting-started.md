---
title: Memulai
description: Jalankan program Aksa pertamamu di browser, di baris perintah, atau di papan sungguhan.
---

Aksa adalah bahasa pemrograman kecil untuk anak. Kata kunci, perintah bawaan, dan pesan error semuanya berasal dari kamus bahasa, sehingga bahasa yang sama terasa alami dalam bahasa Indonesia, Inggris, dan bahasa lain yang ditambahkan nanti.

## Di browser

Cara termudah mencoba Aksa adalah editor browser — tanpa instalasi sama sekali. Buka editornya, pilih pelajaran, tekan **Jalankan**, lalu lihat robot menggambar atau konsol menulis.

```aksa
warna("biru")
ulangi 4 {
    maju(100)
    belok_kanan(90)
}
```

Editor memeriksa seluruh programmu sambil kamu mengetik dan menampilkan setiap kesalahan dalam bahasamu, lengkap dengan barisnya.

Kalau membangun dari sumber, `make web` membangun modul WebAssembly dan bundel editor, lalu sajikan akar repo dan buka `/web/`.

## Di baris perintah

Intinya adalah C99 murni — bangun dengan kompiler C apa pun:

```bash
make                                  # bangun CLI
./aksa run examples/halo.aksa --locale id
./aksa check berkas.aksa              # daftar semua kesalahan, tanpa menjalankan
```

Jalankan `aksa` dari akar repo agar menemukan `locales/`. Lihat [referensi CLI](/id/reference/cli) untuk semua perintah.

## Di papan sungguhan

Program Aksa bisa keluar dari layar: **Kirim ke Perangkat** di editor mengubah programmu menjadi C, mengompilasinya untuk papan ESP32, dan memasangnya lewat USB — langsung dari browser, tanpa instalasi. Papan dikenali otomatis (ESP32-C3 dan C6 didukung saat ini).

[Kit Aksabot](/id/aksabot/overview) akan menjadikan ini pengalaman perangkat keras yang lengkap untuk anak.

## Langkah berikutnya

- Ikuti [pelajaran](/id/learn/lesson-01) — 15 langkah bertingkat dari kata pertama sampai permainan tebak angka.
- Baca [ringkasan bahasa](/id/language/overview) untuk gambaran lengkap.
- Jelajahi [perintah bawaan](/id/reference/builtins) dan [kode error](/id/reference/errors).
