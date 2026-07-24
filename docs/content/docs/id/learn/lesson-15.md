---
title: "15. Proyek: Tebak Angka"
description: "Proyek terakhir!"
---

Proyek terakhir! Komputer menyimpan angka rahasia. Terus tebak sampai benar. Ganti angka rahasianya, lalu tantang temanmu untuk menebak!

## Kode

```aksa
buat rahasia = 7
buat tebakan = 0
selama (tebakan != rahasia) {
    tebakan = tanya("Tebak angka 1 sampai 10:")
    jika (tebakan < rahasia) {
        cetak("Terlalu kecil!")
    } lainnya jika (tebakan > rahasia) {
        cetak("Terlalu besar!")
    }
}
cetak("Hebat! Angkanya", rahasia)
```

Salin kode ini ke editor Aksa, jalankan, lalu ubah sesuatu dan jalankan lagi!
