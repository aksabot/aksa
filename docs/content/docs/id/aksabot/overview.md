---
title: Aksabot
description: Kit perangkat keras Aksa yang akan datang — papan robot kecil yang diprogram anak dalam bahasanya sendiri.
---

> **Status: sedang dikembangkan.** Aksabot sedang dalam tahap prototipe. Semua di halaman ini menggambarkan rencana; detail bisa berubah sebelum rilis.

Aksabot adalah kit perangkat keras tertutup berbasis ESP32-C6, dirancang sebagai langkah kelulusan dari robot layar ke mesin sungguhan — bahasa yang sama, pelajaran yang sama, lampu dan motor sungguhan.

## Tanpa komputer

Papan ini adalah lingkungan koding-nya sendiri. Nyalakan dan ia menjadi hotspot Wi-Fi yang menyajikan editor Aksa — sambungkan dari ponsel, tablet, atau laptop mana pun, cetak program di browser, dan program berjalan langsung di chip. Tanpa aplikasi, tanpa instalasi, tanpa internet.

## Komponen bernama, bukan nomor pin

Anak tidak perlu tahu bahwa LED ada di GPIO 4. Setiap komponen Aksabot punya nama, dan profil papan menghasilkan pengkabelannya otomatis sebagai pembuka program Aksa:

```aksa
buat lampu = 4
buat tombol = 9
```

Sehingga program cukup menulis:

```aksa
jika (baca(tombol) == 1) {
    nyalakan(lampu)
}
```

## Apa yang ditambahkan kit ini

- Tiga perintah bawaan baru untuk penggerak dan sensor sungguhan — daya PWM, sudut servo, dan jarak — lihat [perintah Aksabot](/id/aksabot/builtins).
- Jalur pelajaran perangkat keras yang sejajar dengan 15 pelajaran layar: kedip, tombol, lambaian servo, alarm jarak.
- Editor berbasis teks di v1; editor blok ada di roadmap.

## Jalan menuju produksi

1. Prototipe dari modul yang tersedia di pasaran.
2. Buktikan perangkat lunaknya di ESP32-C6.
3. Validasi kurikulum bersama anak-anak sungguhan.
4. Baru setelah itu: PCB khusus, casing, dan produksi.

Ikuti perkembangannya di [roadmap](/id/reference/roadmap).
