---
title: Tipe & Nilai
description: Angka, teks, dan logika — tiga jenis nilai di Aksa.
---

Aksa sengaja menjaga tipenya minimal: tiga jenis nilai cukup untuk mempelajari semua ide inti pemrograman.

## Angka

Bilangan bulat dan desimal, dengan matematika biasa:

```aksa
buat apel = 4
tulis(apel + 3)     // 7
tulis(apel * 2)     // 8
tulis(apel / 2)     // 2
tulis(apel - 1)     // 3
tulis(apel % 2)     // 0 — sisa pembagian
```

Membagi dengan nol menghasilkan error runtime yang ramah (`E101`), bukan crash.

## Teks

Teks hidup di antara tanda kutip ganda. `+` menyambung teks:

```aksa
buat nama = "Aksa"
tulis("halo " + nama)
```

Teks yang kehilangan kutip penutupnya ditangkap dengan error `E002`.

## Logika

Sesuatu hanya bisa `benar` atau `salah` — tak ada di antaranya:

```aksa
buat sudah_sarapan = benar
buat hujan = salah
jika (sudah_sarapan dan bukan hujan) {
  tulis("Ayo main di luar!")
}
```

Gabungkan logika dengan `dan`, `atau`, dan `bukan`. Perbandingan menghasilkan logika: `==`, `!=`, `<`, `>`, `<=`, `>=`.

## Tanpa "truthy"

Syarat harus bernilai logika. `jika (5)` bukan tebakan "truthy" diam-diam — itu error `E105` dengan penjelasan ramah anak: *"Bagian di dalam ( ) harus bernilai benar atau salah."* Dengan begitu program berarti persis seperti yang tertulis.

## Variabel

Buat variabel dengan `buat` (atau langsung isi — `buat` opsional):

```aksa
buat umur = 9
umur = umur + 1
```

Memakai variabel sebelum membuatnya adalah error `E100`; membuat yang sama dua kali adalah `E111`. Nama variabel menerima Unicode penuh, jadi anak bisa menulis `umur`, `年齢`, atau apa pun dalam aksaranya sendiri.
