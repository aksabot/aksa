#!/bin/sh
# Assemble device/sketch/ for arduino-cli: interpreter core + ESP32 HAL +
# device files, plus a generated header with the locale JSON baked in.
# core/locale.h shadows a toolchain C++ header once the sketch dir is on the
# include path, so locale.* is renamed ak_locale.* on copy (same as the
# compile server does).
set -e
cd "$(dirname "$0")/.."
out=device/sketch
rm -rf "$out"
mkdir -p "$out"
for f in core/error.c core/error.h core/locale.c core/locale.h \
         core/lexer.c core/lexer.h core/ast.c core/ast.h \
         core/parser.c core/parser.h core/checker.c core/checker.h \
         core/vm.c core/vm.h \
         runtime-device/hal.h runtime-device/hal_esp32.cpp \
         device/device_host.cpp device/index_html.h device/aksa_device.ino; do
    name=$(basename "$f")
    case "$name" in
    locale.*) name="ak_$name" ;;
    aksa_device.ino) name=sketch.ino ;; # ino must match the sketch dir name
    esac
    sed 's/#include "locale\.h"/#include "ak_locale.h"/' "$f" >"$out/$name"
done
bun -e '
const { readFileSync, writeFileSync } = require("fs");
const bake = (l) =>
  `static const char aksa_locale_${l}[] = ${JSON.stringify(readFileSync(`locales/${l}.json`, "utf8"))};\n`;
writeFileSync("device/sketch/aksa_locale.h", bake("id") + bake("en"));
'
