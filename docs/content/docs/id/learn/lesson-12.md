---
title: "12. Genap atau Ganjil"
description: "Tanda % memberi sisa pembagian."
---

Tanda % memberi sisa pembagian. Kalau sisa dibagi 2 sama dengan 0, angkanya genap. Trik ini menggabungkan menghitung dan memilih!

## Kode

```aksa
buat n = 1
ulangi 10 {
    jika (n % 2 == 0) {
        cetak(n, "genap")
    } lainnya {
        cetak(n, "ganjil")
    }
    n = n + 1
}
```

Salin kode ini ke editor Aksa, jalankan, lalu ubah sesuatu dan jalankan lagi!
