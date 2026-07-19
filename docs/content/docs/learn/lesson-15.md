---
title: "15. Project: Guess the Number"
description: "The final project!"
---

The final project! The computer keeps a secret number. Keep guessing until you get it right. Change the secret, then challenge a friend to guess it!

## Code

```aksa
make secret = 7
make guess = 0
while (guess != secret) {
    guess = ask("Guess a number from 1 to 10:")
    if (guess < secret) {
        print("Too small!")
    } else if (guess > secret) {
        print("Too big!")
    }
}
print("Great! The number was", secret)
```

Copy this code into the Aksa editor, run it, then change something and run it again!
