---
title: Ringkasan Bahasa
description: Apa itu Aksa, ide di baliknya, dan seperti apa sebuah program.
---

Aksa (dari *aksara* — huruf; Sanskerta *akṣara*, "tak binasa") dibangun di atas tiga ide:

1. **Anak seharusnya membuat kode dalam bahasanya sendiri.** Semua 13 kata kunci, setiap perintah bawaan, dan setiap pesan error berasal dari kamus bahasa. Menambah bahasa cukup menambah satu berkas JSON — tidak ada yang tertanam di kode.
2. **Pesan error adalah produk sesungguhnya.** Setiap kesalahan punya ID tetap, posisi di kode, dan penjelasan ramah anak dalam bahasanya. Seluruh program diperiksa sebelum dijalankan, jadi anak melihat *semua* kesalahannya sekaligus — bukan pesan teknis kompiler.
3. **Tanpa instalasi, lulus ke perangkat nyata.** Intinya C99 murni: berjalan native, terkompilasi ke WebAssembly untuk browser, dan diubah ke C untuk dipasang di papan ESP32 sungguhan — program yang sama dari robot layar sampai LED nyata.

## Program pertama

```aksa
// program pertamaku
fungsi sapa(nama) {
  kembali "halo " + nama
}

ulangi 3 {
  cetak(sapa("dunia"))
}
```

Program yang sama, bahasa yang sama, lokal Inggris:

```aksa
function greet(name) {
  return "hello " + name
}

repeat 3 {
  print(greet("world"))
}
```

## Hal-hal penting

- **Komentar** dimulai dengan `//` sampai akhir baris.
- **Nama** menerima Unicode penuh, jadi anak menamai apa pun dengan kata-katanya sendiri.
- **Berkas** memakai ekstensi `.aksa`.
- **Tipe** ada angka, teks, dan logika — lihat [Tipe & Nilai](/id/language/types).
- Syarat harus bernilai logika: `jika (5)` menghasilkan error yang ramah, bukan tebakan diam-diam.

## Cara berjalannya

```
kode ──► lexer ──► parser ──► pemeriksa ──► VM bytecode   (mode belajar)
      (mengikuti     AST      semua error    └─► robot/simulator di browser
       kamus)                 ditangkap
                              di sini        └─► pengubah C ──► papan nyata
                                                 (mode kirim)
```

Lexer memetakan kata kunci lokal ke token abstrak, pemeriksa menangkap semua error sebelum apa pun berjalan, dan program yang sudah lolos periksa berjalan di VM bytecode (browser atau CLI) atau diubah menjadi C untuk papan sungguhan.
