// Graded lesson content for the language page: title shown in the picker,
// text shown above the editor, code loaded as the starter program.
// Same order in every locale so switching languages keeps the place.
// Grouped basic (1-6) -> middle (7-11) -> advanced (12-15).

export const LESSONS = {
  id: [
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
  ],
  en: [
    // --- Basic ---
    {
      title: '1. Writing',
      text: 'A computer can write anything for you. The \'print\' command shows text on the screen. Run it, then change the text to your own words!',
      code: 'print("Hello, world!")\nprint("I am learning Aksa")\n',
    },
    {
      title: '2. Variables',
      text: 'A variable is like a labeled box that stores something. Make the box with the keyword \'make\', then use its name anywhere. Try changing what is inside!',
      code: 'make name = "Aksa"\nmake age = 8\nprint(name, "is", age, "years old")\n',
    },
    {
      title: '3. Math',
      text: 'A computer is great at math. Use + to add, - to subtract, * to multiply, and / to divide. Try changing the numbers and watch the answers change!',
      code: 'make apples = 4\nmake oranges = 3\nprint("Total fruit:", apples + oranges)\nprint("Double the apples:", apples * 2)\nprint("One less orange:", oranges - 1)\n',
    },
    {
      title: '4. Asking',
      text: 'A program can ask you questions with the \'ask\' command. Your answer is stored in a variable. Run it, then type your answer in the black box below.',
      code: 'name = ask("What is your name?")\nprint("Hello,", name, "! Nice to meet you")\n',
    },
    {
      title: '5. Repeating',
      text: 'Tired of writing the same command again and again? Use \'repeat\'! Everything inside { } runs again. Try changing the number to 10.',
      code: 'repeat 3 {\n    print("Aksa is fun!")\n}\n',
    },
    {
      title: '6. Choosing',
      text: 'With \'if\', a program can choose. When the part inside ( ) is true it runs the first block; otherwise the \'else\' block. Try answering with different numbers!',
      code: 'age = ask("How old are you?")\nif (age >= 10) {\n    print("Wow, you are big!")\n} else {\n    print("You are small, but mighty!")\n}\n',
    },
    // --- Middle ---
    {
      title: '7. Counting',
      text: 'Want to know which loop you are on? Make a counter, then add 1 every time around. This is how you make a counting loop (like a "for" loop)!',
      code: 'make i = 1\nrepeat 5 {\n    print("Step", i)\n    i = i + 1\n}\n',
    },
    {
      title: '8. While',
      text: '\'while\' keeps repeating as long as its condition is true. This program counts down like a rocket. Careful: change the number inside, or it will never stop!',
      code: 'make number = 5\nwhile (number > 0) {\n    print(number)\n    number = number - 1\n}\nprint("Liftoff! 🚀")\n',
    },
    {
      title: '9. True & False',
      text: 'Something can only be true or false — nothing in between. You can store it in a variable and use it to choose. Join them with the keywords \'and\', \'or\', and \'not\'. Try changing \'true\' to \'false\'!',
      code: 'make had_breakfast = true\nmake is_raining = false\nif (had_breakfast and not is_raining) {\n    print("Let\'s play outside!")\n} else {\n    print("Let\'s play indoors instead")\n}\n',
    },
    {
      title: '10. Functions',
      text: 'A function is a command you invent yourself. Write the recipe once with the keyword \'function\', then call its name any time. Try making your own greeting!',
      code: 'function greet(name) {\n    print("Hello,", name, "!")\n}\ngreet("Aksa")\ngreet("Bima")\ngreet("Mom")\n',
    },
    {
      title: '11. Returning Answers',
      text: 'A function can also give back an answer with the keyword \'return\'. Call the function, then use its answer. Try making your own math function!',
      code: 'function area(side) {\n    return side * side\n}\nprint("Area of a 5 square:", area(5))\nprint("Area of a 10 square:", area(10))\n',
    },
    // --- Advanced ---
    {
      title: '12. Even or Odd',
      text: 'The % sign gives the remainder of a division. If a number divided by 2 leaves 0, it is even. This trick mixes counting and choosing!',
      code: 'make n = 1\nrepeat 10 {\n    if (n % 2 == 0) {\n        print(n, "is even")\n    } else {\n        print(n, "is odd")\n    }\n    n = n + 1\n}\n',
    },
    {
      title: '13. Robot',
      text: 'The robot draws while it walks. \'forward\' moves it, \'turn_right\' spins it. Four forwards and turns make a square. Try changing the numbers!',
      code: 'color("blue")\nrepeat 4 {\n    forward(100)\n    turn_right(90)\n}\n',
    },
    {
      title: '14. Project: Spiral',
      text: 'Project time! If every step is a little longer than the last, you get a spiral. Change the color, numbers, and turns — make a spiral that is truly yours!',
      code: 'make step = 5\ncolor("purple")\nrepeat 30 {\n    forward(step)\n    turn_right(25)\n    step = step + 3\n}\n',
    },
    {
      title: '15. Project: Guess the Number',
      text: 'The final project! The computer keeps a secret number. Keep guessing until you get it right. Change the secret, then challenge a friend to guess it!',
      code: 'make secret = 7\nmake guess = 0\nwhile (guess != secret) {\n    guess = ask("Guess a number from 1 to 10:")\n    if (guess < secret) {\n        print("Too small!")\n    } else if (guess > secret) {\n        print("Too big!")\n    }\n}\nprint("Great! The number was", secret)\n',
    },
  ],
};
