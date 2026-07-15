#!/bin/sh
# Differential tests: emitted C must compile (-Wall -Werror) and behave like
# the VM. Each tests/golden/ok program, emitted and compiled, must exit 0 with
# stdout equal to its .out. Each err program must surface the same "Exxx L"
# pairs as the VM — statically (when `aksa emit` refuses, same front-end) or at
# runtime (from the compiled program). E109 (too deep) is excluded: a C stack
# overflow can't mimic the VM's frame-limit error.
set -u
cd "$(dirname "$0")/.."
S="${TMPDIR:-/tmp}/aksa-diff.$$"
mkdir -p "$S"
trap 'rm -rf "$S"' EXIT

CC=${CC:-cc}
RT="core/rt.c runtime-device/hal_sim.c core/error.c core/locale.c"
fail=0 total=0

compile() { $CC -std=c99 -Wall -Werror -Icore -Iruntime-device "$1" $RT -lm -o "$2"; }
ids() { sed -n 's/^! \(E[0-9][0-9]*\) \([0-9][0-9]*\):.*/\1 \2/p'; }

for f in tests/golden/ok/*.aksa; do
    total=$((total + 1))
    base=${f%.aksa}
    loc=${base##*.}
    in=/dev/null
    [ -f "$base.in" ] && in="$base.in"
    if ! ./aksa emit "$f" --locale "$loc" >"$S/e.c" 2>/dev/null; then
        fail=$((fail + 1))
        echo "FAIL $f (emit refused a valid program)"
        continue
    fi
    if ! compile "$S/e.c" "$S/p" 2>"$S/cc.err"; then
        fail=$((fail + 1))
        printf 'FAIL %s (emitted C did not compile)\n%s\n' "$f" "$(cat "$S/cc.err")"
        continue
    fi
    got=$("$S/p" <"$in")
    rc=$?
    want=$(cat "$base.out")
    if [ $rc -ne 0 ] || [ "$got" != "$want" ]; then
        fail=$((fail + 1))
        printf 'FAIL %s (exit %d)\n--- want ---\n%s\n--- got ----\n%s\n' \
            "$f" "$rc" "$want" "$got"
    fi
done

for f in tests/golden/err/*.aksa; do
    base=${f%.aksa}
    loc=${base##*.}
    want=$(cat "$base.err")
    case "$want" in *E109*) continue ;; esac # excluded, see header
    total=$((total + 1))
    in=/dev/null
    [ -f "$base.in" ] && in="$base.in"
    if ! ./aksa emit "$f" --locale "$loc" >"$S/e.c" 2>"$S/emit.err"; then
        got=$(ids <"$S/emit.err") # static error: same front-end as the VM
    else
        if ! compile "$S/e.c" "$S/p" 2>"$S/cc.err"; then
            fail=$((fail + 1))
            printf 'FAIL %s (emitted C did not compile)\n%s\n' "$f" "$(cat "$S/cc.err")"
            continue
        fi
        err=$("$S/p" <"$in" 2>&1 >/dev/null)
        rc=$?
        if [ $rc -eq 0 ]; then
            fail=$((fail + 1))
            echo "FAIL $f (runtime error expected, program exited 0)"
            continue
        fi
        got=$(printf '%s\n' "$err" | ids)
    fi
    if [ "$got" != "$want" ]; then
        fail=$((fail + 1))
        printf 'FAIL %s\n--- want ---\n%s\n--- got ----\n%s\n' "$f" "$want" "$got"
    fi
done

echo "tests/diff: $total programs, $fail failed"
[ "$fail" -eq 0 ]
