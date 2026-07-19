---
title: "12. Even or Odd"
description: "The % sign gives the remainder of a division."
---

The % sign gives the remainder of a division. If a number divided by 2 leaves 0, it is even. This trick mixes counting and choosing!

## Code

```aksa
make n = 1
repeat 10 {
    if (n % 2 == 0) {
        print(n, "is even")
    } else {
        print(n, "is odd")
    }
    n = n + 1
}
```

Copy this code into the Aksa editor, run it, then change something and run it again!
