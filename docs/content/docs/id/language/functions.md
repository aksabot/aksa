---
title: Fungsi
description: Membuat perintahmu sendiri dengan fungsi dan kembali.
---

Fungsi adalah perintah buatanmu sendiri. Tulis resepnya sekali, lalu panggil namanya kapan saja:

```aksa
fungsi sapa(nama) {
    tulis("Halo,", nama, "!")
}
sapa("Aksa")
sapa("Bima")
sapa("Ibu")
```

## Mengembalikan jawaban

Fungsi bisa memberi jawaban dengan `kembali`:

```aksa
fungsi luas(sisi) {
    kembali sisi * sisi
}
tulis("Luas kotak sisi 5:", luas(5))
```

Fungsi bisa memanggil dirinya sendiri dan fungsi lain — rekursi bekerja:

```aksa
fungsi fib(n) {
    jika (n < 2) { kembali n }
    kembali fib(n - 1) + fib(n - 2)
}
tulis(fib(10))
```

## Aturan, diperiksa sebelum apa pun berjalan

Setiap aturan di bawah ditangkap pemeriksa dengan pesan ramah dalam bahasamu — sebelum program mulai:

- Memanggil fungsi dengan jumlah isian yang salah adalah error `E103`.
- `kembali` di luar fungsi adalah error `E108`.
- Memanggil nama yang tidak ada adalah error `E102`.
- Program yang terlalu dalam (rekursi tak terkendali) berhenti dengan aman lewat error `E109`.
