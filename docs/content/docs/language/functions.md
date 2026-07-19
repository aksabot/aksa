---
title: Functions
description: Defining your own commands with function and return.
---

A function is a command you invent yourself. Write the recipe once, then call its name any time:

```aksa
function greet(name) {
    print("Hello,", name, "!")
}
greet("Aksa")
greet("Bima")
greet("Mom")
```

## Returning answers

A function can give back an answer with `return`:

```aksa
function area(side) {
    return side * side
}
print("Area of a 5 square:", area(5))
```

Functions can call themselves and each other — recursion works:

```aksa
function fib(n) {
    if (n < 2) { return n }
    return fib(n - 1) + fib(n - 2)
}
print(fib(10))
```

## Rules, checked before anything runs

Every rule below is caught by the checker with a friendly, localized message — before the program starts:

- Calling a function with the wrong number of inputs is error `E103`.
- `return` outside a function is error `E108`.
- Calling a name that does not exist is error `E102`.
- A program that goes too deep (runaway recursion) stops safely with error `E109`.
