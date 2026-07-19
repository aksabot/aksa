---
title: Tata Bahasa
description: Tata bahasa formal Aksa, untuk yang penasaran.
---

Tata bahasa Aksa cukup kecil untuk dibaca sekali duduk. Kata kunci di bawah bersifat abstrak — setiap lokal menyediakan katanya sendiri.

```
program     := statement*
statement   := if_stmt | repeat_stmt | while_stmt | func_def
             | return_stmt | break_stmt | assign_stmt | expr_stmt
if_stmt     := IF "(" expr ")" block (ELSE (if_stmt | block))?
repeat_stmt := REPEAT expr block            // ulangi 10 { ... }
while_stmt  := WHILE "(" expr ")" block
func_def    := FUNCTION IDENT "(" params? ")" block
assign_stmt := (VAR)? IDENT "=" expr
block       := "{" statement* "}"
expr        := urutan prioritas standar: or → and → not → perbandingan
               (==, !=, <, >, <=, >=) → tambah/kurang → kali/bagi/sisa → unary → panggilan/primer
```

## Catatan

- Kurung kurawal dan biasa sengaja bergaya C — Aksa lulus ke C sungguhan di perangkat keras, dan bentuknya sebaiknya terasa akrab.
- `VAR` (`buat` / `make`) opsional: mengisi nama baru otomatis membuatnya.
- Nama boleh memakai huruf Unicode penuh, jadi variabel dan fungsi bisa dinamai dalam bahasa apa pun.
- Komentar berjalan dari `//` sampai akhir baris.
- Parser-nya recursive descent dengan pemulihan panik: ia melaporkan beberapa error sekaligus alih-alih berhenti di error pertama.
