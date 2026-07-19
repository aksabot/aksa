---
title: Types & Values
description: Numbers, strings, and booleans — the three kinds of values in Aksa.
---

Aksa keeps types minimal on purpose: three kinds of values are enough to learn every core idea of programming.

## Numbers

Whole numbers and decimals, with the usual math:

```aksa
make apples = 4
print(apples + 3)     // 7
print(apples * 2)     // 8
print(apples / 2)     // 2
print(apples - 1)     // 3
print(apples % 2)     // 0 — remainder of a division
```

Dividing by zero is a friendly runtime error (`E101`), never a crash.

## Strings

Text lives between double quotes. `+` joins strings together:

```aksa
make name = "Aksa"
print("hello " + name)
```

A string that is missing its closing quote is caught with error `E002`.

## Booleans

Something is either `true` or `false` — nothing in between:

```aksa
make had_breakfast = true
make is_raining = false
if (had_breakfast and not is_raining) {
  print("Let's play outside!")
}
```

Combine booleans with `and`, `or`, and `not`. Comparisons produce booleans: `==`, `!=`, `<`, `>`, `<=`, `>=`.

## No truthiness

Conditions must be boolean. `if (5)` is not a silent "truthy" guess — it is error `E105` with a kid-friendly explanation: *"The part inside ( ) must be true or false."* This makes programs mean exactly what they say.

## Variables

Create a variable with `make` (or just assign — `make` is optional):

```aksa
make age = 9
age = age + 1
```

Using a variable before making it is error `E100`; making the same one twice is `E111`. Variable names accept full Unicode, so kids can write `umur`, `年齢`, or anything in their own script.
