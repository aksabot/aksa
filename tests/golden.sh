#!/bin/sh
# Golden tests. Each tests/golden/ok/<name>.<locale>.aksa runs with its
# locale and must exit 0 with stdout equal to <name>.<locale>.out.
# Each tests/golden/err/<name>.<locale>.aksa must exit non-zero and its
# stderr error lines ("! Exxx L:C msg") must match the "Exxx L" pairs in
# <name>.<locale>.err. An optional <name>.<locale>.in is fed to stdin.
set -u
cd "$(dirname "$0")/.."
fail=0 total=0

for f in tests/golden/ok/*.aksa; do
    total=$((total + 1))
    base=${f%.aksa}
    loc=${base##*.}
    in=/dev/null
    [ -f "$base.in" ] && in="$base.in"
    got=$(./aksa run "$f" --locale "$loc" <"$in" 2>/dev/null)
    rc=$?
    want=$(cat "$base.out")
    if [ $rc -ne 0 ] || [ "$got" != "$want" ]; then
        fail=$((fail + 1))
        printf 'FAIL %s (exit %d)\n--- want ---\n%s\n--- got ----\n%s\n' \
            "$f" "$rc" "$want" "$got"
    fi
done

for f in tests/golden/err/*.aksa; do
    total=$((total + 1))
    base=${f%.aksa}
    loc=${base##*.}
    in=/dev/null
    [ -f "$base.in" ] && in="$base.in"
    err=$(./aksa run "$f" --locale "$loc" <"$in" 2>&1 >/dev/null)
    rc=$?
    got=$(printf '%s\n' "$err" |
        sed -n 's/^! \(E[0-9][0-9]*\) \([0-9][0-9]*\):.*/\1 \2/p')
    want=$(cat "$base.err")
    if [ $rc -eq 0 ] || [ "$got" != "$want" ]; then
        fail=$((fail + 1))
        printf 'FAIL %s (exit %d)\n--- want ---\n%s\n--- got ----\n%s\n' \
            "$f" "$rc" "$want" "$got"
    fi
done

echo "tests/golden: $total programs, $fail failed"
[ "$fail" -eq 0 ]
