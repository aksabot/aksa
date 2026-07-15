// Graded lesson content for the language page: title shown in the picker,
// text shown above the editor, code loaded as the starter program.
// Same order in every locale so switching languages keeps the place.

export const LESSONS = {
  id: [
    {
      title: '1. Menulis',
      text: 'Komputer bisa menulis apa saja untukmu. Perintah tulis menampilkan teks di layar. Coba jalankan, lalu ubah teksnya menjadi kata-katamu sendiri!',
      code: 'tulis("Halo, dunia!")\ntulis("Aku sedang belajar Aksa")\n',
    },
    {
      title: '2. Variabel',
      text: 'Variabel itu seperti kotak berlabel untuk menyimpan sesuatu. Buat kotaknya dengan buat, lalu pakai namanya di mana saja. Coba ganti isinya!',
      code: 'buat nama = "Kila"\nbuat umur = 8\ntulis(nama, "berumur", umur, "tahun")\n',
    },
    {
      title: '3. Bertanya',
      text: 'Program bisa bertanya kepadamu dengan tanya. Jawabanmu tersimpan di variabel. Jalankan, lalu ketik jawabanmu di kotak hitam bawah.',
      code: 'nama = tanya("Siapa namamu?")\ntulis("Halo,", nama, "! Senang bertemu denganmu")\n',
    },
    {
      title: '4. Mengulang',
      text: 'Malas menulis perintah yang sama berkali-kali? Pakai ulangi! Semua yang ada di dalam { } akan diulang. Coba ubah angkanya menjadi 10.',
      code: 'ulangi 3 {\n    tulis("Aksa itu seru!")\n}\n',
    },
    {
      title: '5. Memilih',
      text: 'Dengan jika, program bisa memilih. Kalau bagian dalam ( ) benar, jalankan blok pertama; kalau tidak, blok lainnya. Coba jawab dengan angka berbeda!',
      code: 'umur = tanya("Berapa umurmu?")\njika (umur >= 10) {\n    tulis("Wah, kamu sudah besar!")\n} lainnya {\n    tulis("Kamu masih kecil, tapi hebat!")\n}\n',
    },
    {
      title: '6. Selama',
      text: 'selama mengulang terus selama syaratnya masih benar. Program ini menghitung mundur seperti roket. Awas, jangan lupa mengubah angkanya, atau tidak akan pernah berhenti!',
      code: 'buat angka = 5\nselama (angka > 0) {\n    tulis(angka)\n    angka = angka - 1\n}\ntulis("Meluncur! 🚀")\n',
    },
    {
      title: '7. Fungsi',
      text: 'Fungsi adalah perintah buatanmu sendiri. Tulis resepnya sekali dengan fungsi, lalu panggil namanya kapan saja. Coba buat fungsi sapaanmu sendiri!',
      code: 'fungsi sapa(nama) {\n    tulis("Halo,", nama, "!")\n}\nsapa("Kila")\nsapa("Bima")\nsapa("Ibu")\n',
    },
    {
      title: '8. Kura-kura',
      text: 'Kura-kura menggambar sambil berjalan. maju menggerakkannya, belok_kanan memutarnya. Empat kali maju dan belok membuat kotak. Coba ubah angkanya!',
      code: 'warna("biru")\nulangi 4 {\n    maju(100)\n    belok_kanan(90)\n}\n',
    },
    {
      title: '9. Proyek: Spiral',
      text: 'Saatnya proyek! Kalau setiap langkah sedikit lebih panjang dari sebelumnya, jadilah spiral. Ubah warna, angka, dan belokannya — buat spiral khasmu sendiri!',
      code: 'buat langkah = 5\nwarna("ungu")\nulangi 30 {\n    maju(langkah)\n    belok_kanan(25)\n    langkah = langkah + 3\n}\n',
    },
  ],
  en: [
    {
      title: '1. Writing',
      text: 'A computer can write anything for you. The print command shows text on the screen. Run it, then change the text to your own words!',
      code: 'print("Hello, world!")\nprint("I am learning Aksa")\n',
    },
    {
      title: '2. Variables',
      text: 'A variable is like a labeled box that stores something. Make the box with make, then use its name anywhere. Try changing what is inside!',
      code: 'make name = "Kila"\nmake age = 8\nprint(name, "is", age, "years old")\n',
    },
    {
      title: '3. Asking',
      text: 'A program can ask you questions with ask. Your answer is stored in a variable. Run it, then type your answer in the black box below.',
      code: 'name = ask("What is your name?")\nprint("Hello,", name, "! Nice to meet you")\n',
    },
    {
      title: '4. Repeating',
      text: 'Tired of writing the same command again and again? Use repeat! Everything inside { } runs again. Try changing the number to 10.',
      code: 'repeat 3 {\n    print("Aksa is fun!")\n}\n',
    },
    {
      title: '5. Choosing',
      text: 'With if, a program can choose. When the part inside ( ) is true it runs the first block; otherwise the else block. Try answering with different numbers!',
      code: 'age = ask("How old are you?")\nif (age >= 10) {\n    print("Wow, you are big!")\n} else {\n    print("You are small, but mighty!")\n}\n',
    },
    {
      title: '6. While',
      text: 'while keeps repeating as long as its condition is true. This program counts down like a rocket. Careful: change the number inside, or it will never stop!',
      code: 'make number = 5\nwhile (number > 0) {\n    print(number)\n    number = number - 1\n}\nprint("Liftoff! 🚀")\n',
    },
    {
      title: '7. Functions',
      text: 'A function is a command you invent yourself. Write the recipe once with function, then call its name any time. Try making your own greeting!',
      code: 'function greet(name) {\n    print("Hello,", name, "!")\n}\ngreet("Kila")\ngreet("Bima")\ngreet("Mom")\n',
    },
    {
      title: '8. Turtle',
      text: 'The turtle draws while it walks. forward moves it, turn_right spins it. Four forwards and turns make a square. Try changing the numbers!',
      code: 'color("blue")\nrepeat 4 {\n    forward(100)\n    turn_right(90)\n}\n',
    },
    {
      title: '9. Project: Spiral',
      text: 'Project time! If every step is a little longer than the last, you get a spiral. Change the color, numbers, and turns — make a spiral that is truly yours!',
      code: 'make step = 5\ncolor("purple")\nrepeat 30 {\n    forward(step)\n    turn_right(25)\n    step = step + 3\n}\n',
    },
  ],
};
