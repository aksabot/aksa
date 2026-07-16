#!/bin/sh
# Leak check for emitted C: strings are reference-counted (core/rt.c), so a
# program that builds text in a loop must free it. mem.id.aksa accumulates one
# string in a function local and discards a fresh concatenation every iteration;
# once the function returns, every heap string must be gone. Built with
# -DAK_TRACK, the runtime counts live heap strings and prints AK_LIVE=<n> at
# exit — anything but 0 is a leak (or a double-free would crash first).
set -u
cd "$(dirname "$0")/.."
S="${TMPDIR:-/tmp}/aksa-mem.$$"
mkdir -p "$S"
trap 'rm -rf "$S"' EXIT

CC=${CC:-cc}
RT="core/rt.c runtime-device/hal_sim.c core/error.c core/locale.c"

./aksa emit tests/mem.id.aksa --locale id >"$S/e.c" || { echo "FAIL emit"; exit 1; }
$CC -std=c99 -Wall -Werror -DAK_TRACK -Icore -Iruntime-device \
    "$S/e.c" $RT -lm -o "$S/p" || { echo "FAIL compile"; exit 1; }

live=$("$S/p" 2>&1 >/dev/null | sed -n 's/^AK_LIVE=//p')
if [ "$live" = "0" ]; then
    echo "tests/mem: no leak (AK_LIVE=0)"
else
    echo "tests/mem: FAIL — leaked heap strings (AK_LIVE=$live)"
    exit 1
fi
