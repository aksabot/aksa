# Changelog

## 0.0.3 — 2026-07-16

### Changed

- Lessons now live in one file per language, making them easier to edit.
- The app's code now checks itself for mistakes before every release.

## 0.0.2 — 2026-07-15

### Added

- 76 example programs now run automatically to prove Aksa behaves exactly as promised.
- Programs now run right in the browser: press Run and watch the output appear.
- Questions from your program show a typing box in the page — no popups.
- A Stop button, and endless loops can never freeze the page.
- A real code editor: colors for keywords and instant underlines where something is wrong.
- Mistakes are listed in your language; click one to jump to the line.
- Turtle drawing! Make shapes with maju/belok, pick colors, and set the drawing speed.
- A virtual circuit board: lights, a button, a slider, a buzzer, and a fan you control from code.
- Programs can now pause with tunggu/wait — the page stays alive and Stop always works.
- A dropdown of ready-made projects, from blinking a light to a reaction game.
- Programs can now turn into real C code — the first step toward running on a device.
- That C code is checked to behave exactly like the app, so nothing changes when you deploy.
- A "Send to Device" button puts your program onto a real ESP32 board, straight from the browser.
- A helper server builds your program into device firmware — nothing to install on your computer.
- Lessons grew from 9 to 15, spanning easy, medium, and hard — including math, counting loops, and a guess-the-number game.
- After sending, messages from the board appear live in the page, mistakes explained in your language.
- Nine step-by-step lessons, from writing your first words to drawing your own spiral.
- A Share button copies a link that opens your exact program on a friend's screen.

### Changed

- Comments now start with // instead of #, matching the C family.
- The app now has two pages: one for learning the language, one for the circuit board and device.
- The drawing arrow is now a little robot you can watch drive around and turn.

## 0.0.1 — 2026-07-14

### Added

- Aksa can now read code and list its building blocks, in Indonesian or English.
- Works on the computer (aksa command) and in the browser with nothing to install.
- Friendly, translated messages when something in the code can't be read.
- Aksa now understands full program structure: if/else, loops, and functions.
- When brackets are missing, Aksa says which one and where — and keeps checking the rest.
- Programs now run! Loops, decisions, and your own functions all work with "aksa run".
- You can ask questions in your program (tanya) and show answers (tulis).
- Mistakes while running (like dividing by zero) are explained simply, in your language.
- Aksa now checks your whole program before running and lists every mistake it finds.
- It catches typos, missing variables, and wrong inputs — all at once, before anything runs.
