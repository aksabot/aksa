// Indonesian lessons. Same order as every other locale so switching keeps the place.
// Grouped basic (1-6) -> middle (7-11) -> advanced (12-15).

import type { Lesson } from './lessons.ts';

export const LESSONS_ID: Lesson[] = [
  // --- Dasar ---
  {
    title: '1. Menulis',
    text: 'Komputer bisa menulis apa saja untukmu. Perintah \'tulis\' menampilkan teks di layar. Coba jalankan, lalu ubah teksnya menjadi kata-katamu sendiri!',
    code: 'tulis("Halo, dunia!")\ntulis("Aku sedang belajar Aksa")\n',
  },
  {
    title: '2. Variabel',
    text: 'Variabel itu seperti kotak berlabel untuk menyimpan sesuatu. Buat kotaknya dengan kata kunci \'buat\', lalu pakai namanya di mana saja. Coba ganti isinya!',
    code: 'buat nama = "Aksa"\nbuat umur = 8\ntulis(nama, "berumur", umur, "tahun")\n',
  },
  {
    title: '3. Berhitung',
    text: 'Komputer jago berhitung. Pakai + untuk tambah, - untuk kurang, * untuk kali, dan / untuk bagi. Coba ganti angkanya dan lihat hasilnya berubah!',
    code: 'buat apel = 4\nbuat jeruk = 3\ntulis("Jumlah buah:", apel + jeruk)\ntulis("Dua kali apel:", apel * 2)\ntulis("Sisa jeruk:", jeruk - 1)\n',
  },
  {
    title: '4. Bertanya',
    text: 'Program bisa bertanya kepadamu dengan perintah \'tanya\'. Jawabanmu tersimpan di variabel. Jalankan, lalu ketik jawabanmu di kotak hitam bawah.',
    code: 'nama = tanya("Siapa namamu?")\ntulis("Halo,", nama, "! Senang bertemu denganmu")\n',
  },
  {
    title: '5. Mengulang',
    text: 'Malas menulis perintah yang sama berkali-kali? Pakai \'ulangi\'! Semua yang ada di dalam { } akan diulang. Coba ubah angkanya menjadi 10.',
    code: 'ulangi 3 {\n    tulis("Aksa itu seru!")\n}\n',
  },
  {
    title: '6. Memilih',
    text: 'Dengan \'jika\', program bisa memilih. Kalau bagian dalam ( ) benar, jalankan blok pertama; kalau tidak, blok \'lainnya\'. Coba jawab dengan angka berbeda!',
    code: 'umur = tanya("Berapa umurmu?")\njika (umur >= 10) {\n    tulis("Wah, kamu sudah besar!")\n} lainnya {\n    tulis("Kamu masih kecil, tapi hebat!")\n}\n',
  },
  // --- Menengah ---
  {
    title: '7. Menghitung',
    text: 'Mau tahu sedang di putaran ke berapa? Buat sebuah penghitung, lalu tambah 1 di setiap putaran. Inilah cara membuat pengulangan berangka (seperti "for")!',
    code: 'buat i = 1\nulangi 5 {\n    tulis("Langkah ke", i)\n    i = i + 1\n}\n',
  },
  {
    title: '8. Selama',
    text: '\'selama\' mengulang terus selama syaratnya masih benar. Program ini menghitung mundur seperti roket. Awas, jangan lupa mengubah angkanya, atau tidak akan pernah berhenti!',
    code: 'buat angka = 5\nselama (angka > 0) {\n    tulis(angka)\n    angka = angka - 1\n}\ntulis("Meluncur! 🚀")\n',
  },
  {
    title: '9. Benar & Salah',
    text: 'Sesuatu hanya bisa benar atau salah — tak ada di antaranya. Kamu bisa menyimpannya di variabel, lalu memakainya untuk memilih. Gabungkan dengan kata kunci \'dan\', \'atau\', dan \'bukan\'. Coba ganti \'benar\' menjadi \'salah\'!',
    code: 'buat sudah_sarapan = benar\nbuat hujan = salah\njika (sudah_sarapan dan bukan hujan) {\n    tulis("Ayo main di luar!")\n} lainnya {\n    tulis("Kita main di dalam saja")\n}\n',
  },
  {
    title: '10. Fungsi',
    text: 'Fungsi adalah perintah buatanmu sendiri. Tulis resepnya sekali dengan kata kunci \'fungsi\', lalu panggil namanya kapan saja. Coba buat fungsi sapaanmu sendiri!',
    code: 'fungsi sapa(nama) {\n    tulis("Halo,", nama, "!")\n}\nsapa("Aksa")\nsapa("Bima")\nsapa("Ibu")\n',
  },
  {
    title: '11. Jawaban Fungsi',
    text: 'Fungsi juga bisa mengembalikan jawaban dengan kata kunci \'kembali\'. Panggil fungsinya, lalu pakai jawabannya. Coba buat fungsi hitungmu sendiri!',
    code: 'fungsi luas(sisi) {\n    kembali sisi * sisi\n}\ntulis("Luas kotak sisi 5:", luas(5))\ntulis("Luas kotak sisi 10:", luas(10))\n',
  },
  // --- Lanjutan ---
  {
    title: '12. Genap atau Ganjil',
    text: 'Tanda % memberi sisa pembagian. Kalau sisa dibagi 2 sama dengan 0, angkanya genap. Trik ini menggabungkan menghitung dan memilih!',
    code: 'buat n = 1\nulangi 10 {\n    jika (n % 2 == 0) {\n        tulis(n, "genap")\n    } lainnya {\n        tulis(n, "ganjil")\n    }\n    n = n + 1\n}\n',
  },
  {
    title: '13. Robot',
    text: 'Robot menggambar sambil berjalan. \'maju\' menggerakkannya, \'belok_kanan\' memutarnya. Empat kali maju dan belok membuat kotak. Coba ubah angkanya!',
    code: 'warna("biru")\nulangi 4 {\n    maju(100)\n    belok_kanan(90)\n}\n',
  },
  {
    title: '14. Proyek: Spiral',
    text: 'Saatnya proyek! Kalau setiap langkah sedikit lebih panjang dari sebelumnya, jadilah spiral. Ubah warna, angka, dan belokannya — buat spiral khasmu sendiri!',
    code: 'buat langkah = 5\nwarna("ungu")\nulangi 30 {\n    maju(langkah)\n    belok_kanan(25)\n    langkah = langkah + 3\n}\n',
  },
  {
    title: '15. Proyek: Tebak Angka',
    text: 'Proyek terakhir! Komputer menyimpan angka rahasia. Terus tebak sampai benar. Ganti angka rahasianya, lalu tantang temanmu untuk menebak!',
    code: 'buat rahasia = 7\nbuat tebakan = 0\nselama (tebakan != rahasia) {\n    tebakan = tanya("Tebak angka 1 sampai 10:")\n    jika (tebakan < rahasia) {\n        tulis("Terlalu kecil!")\n    } lainnya jika (tebakan > rahasia) {\n        tulis("Terlalu besar!")\n    }\n}\ntulis("Hebat! Angkanya", rahasia)\n',
  },
];
