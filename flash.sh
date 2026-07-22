#!/bin/sh
# Flash one Aksa program straight to a plugged-in board from the terminal —
# no browser, no compile server (that lives on a VPS and can't see your USB).
# Emits C with ./aksa, assembles an Arduino sketch (same layout as the compile
# server), then compiles + uploads with arduino-cli.
#   ./flash.sh program.aksa [locale] [board] [port]
# Defaults: locale=id  board=c6  port=first /dev/cu.usbmodem*
set -e
cd "$(dirname "$0")"

file=${1:?usage: ./flash.sh <file.aksa> [locale] [board] [port]}
locale=${2:-id}
board=${3:-c6}
port=${4:-$(ls /dev/cu.usbmodem* 2>/dev/null | head -1)}

case "$board" in
  c3) fqbn=esp32:esp32:esp32c3 ;;
  c6) fqbn=esp32:esp32:esp32c6 ;;
  *) echo "unknown board '$board' (use c3 or c6)"; exit 2 ;;
esac
[ -n "$port" ] || { echo "no board found — plug in the ESP32 (or pass the port as arg 4)"; exit 1; }
[ -x ./aksa ] || { echo "missing ./aksa — run 'make aksa' first"; exit 1; }

# Runtime files for the emitted program (rt.c core + esp32 HAL). locale.* is
# renamed ak_locale.* because "locale.h" shadows a toolchain C++ header once
# the sketch dir is on the include path (same as device/build.sh).
sketch=$(mktemp -d)/aksa
mkdir -p "$sketch"
for f in core/rt.c core/rt.h core/error.c core/error.h \
         core/locale.c core/locale.h core/lexer.h \
         runtime-device/hal.h runtime-device/hal_esp32.cpp; do
    name=$(basename "$f")
    case "$name" in locale.*) name="ak_$name" ;; esac
    sed 's/#include "locale\.h"/#include "ak_locale.h"/' "$f" >"$sketch/$name"
done

printf 'extern "C" int aksa_main(void);\nvoid setup() { aksa_main(); }\nvoid loop() {}\n' >"$sketch/aksa.ino"
{ printf '#define main aksa_main\n'; ./aksa emit "$file" --locale "$locale"; } >"$sketch/program.c"
bun -e "const {readFileSync,writeFileSync}=require('fs');writeFileSync('$sketch/aksa_locale.h',\`static const char aksa_locale_json[] = \${JSON.stringify(readFileSync('locales/$locale.json','utf8'))};\n\`)"

echo "compiling for $board ($fqbn) ..."
arduino-cli compile --fqbn "$fqbn" "$sketch"
echo "uploading to $port ..."
arduino-cli upload --fqbn "$fqbn" -p "$port" "$sketch"
echo "done — board flashed."
