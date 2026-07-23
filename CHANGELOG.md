# Changelog

## 0.0.8 — 2026-07-24

### Fixed

- Long-running programs no longer slow the browser: the output box keeps only recent lines.

## 0.0.7 — 2026-07-23

### Changed

- `tunggu` now waits in seconds, not milliseconds: `tunggu(1)` = 1 second, `tunggu(0.1)` = a blink.

## 0.0.6 — 2026-07-22

### Added

- The board can now be its own Wi-Fi hotspot: join it, open a browser, and code — no computer needed.
- The page now shows output live as the program runs, with a Stop button.
- Programs can loop forever (watch a button, a sensor) and keep printing until you tap Stop.
- The board's Wi-Fi now has a password (same as the network name) and allows one device at a time.
- New pin map: 15 starter parts (LED, button, sensors, servo, display) each get a name to code with.

### Fixed

- The board's example program now blinks the correct light (GPIO15) on the C6.
- Loops (`ulangi`) no longer crash the board when coding straight on it.
- The Stop button now stops a program even while it is waiting (`tunggu`).
- The board no longer gets stuck on "busy" if a program fails to start.
- If the board stops responding, the page now says so instead of freezing on "...".
- Very deeply nested programs now show a clear error instead of crashing the board.

## 0.0.5 — 2026-07-21

### Added

- Send a program to a real board straight from the terminal: `make aksa-flash FILE=your.aksa`.

### Fixed

- Sending to an ESP32-C6 board from the browser no longer drops the connection mid-way.
- One command to start the tool that sends your code to a real board: `make server`.

## 0.0.4 — 2026-07-19

### Added

- A documentation website: guides, all 15 lessons, and every command — in English and Indonesian.
- The docs also introduce Aksabot, the upcoming hardware kit, and its planned commands.

### Changed

- The coding playground has a fresh, playful look: rounded buttons, bright colors, kid-friendly fonts.
- The drawing robot on the canvas now looks like the Aksa logo robot.
- The playground now publishes automatically to play.aksabot.com.
- The docs and the playground now link to each other.
- The documentation now opens in Indonesian by default.

## 0.0.3 — 2026-07-16

### Added

- Send to Device now recognizes the board you plug in (C3 or C6) automatically.
- The board can be its own computer: join its Wi-Fi from any phone and code — no PC needed.

### Fixed

- Programs that build text in a loop no longer run out of memory on the board.

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
