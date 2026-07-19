---
title: Control Flow
description: Choosing with if/else, repeating with repeat and while, and stopping loops.
---

## Choosing: `if` / `else`

The part inside `( )` must be true or false. When it is true the first block runs; otherwise the `else` block:

```aksa
age = ask("How old are you?")
if (age >= 10) {
    print("Wow, you are big!")
} else {
    print("You are small, but mighty!")
}
```

Chains work the way you expect:

```aksa
if (guess < secret) {
    print("Too small!")
} else if (guess > secret) {
    print("Too big!")
} else {
    print("You got it!")
}
```

## Repeating a fixed number of times: `repeat`

`repeat` takes a number and runs the block that many times:

```aksa
repeat 4 {
    forward(100)
    turn_right(90)
}
```

Giving `repeat` something that is not a number is error `E110`.

## Repeating while true: `while`

`while` keeps going as long as its condition is true:

```aksa
make number = 5
while (number > 0) {
    print(number)
    number = number - 1
}
print("Liftoff!")
```

In the browser, endless loops can never freeze the page — the runtime yields regularly and the Stop button always works.

## Leaving a loop: `stop`

`stop` breaks out of the current loop immediately:

```aksa
i = 3
while (i > 0) {
    print(i)
    i = i - 1
    if (i == 1) { stop }
}
```

Using `stop` outside a loop is error `E107`.
