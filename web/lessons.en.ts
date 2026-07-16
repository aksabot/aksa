// English lessons. Same order as every other locale so switching keeps the place.
// Grouped basic (1-6) -> middle (7-11) -> advanced (12-15).

import type { Lesson } from './lessons.ts';

export const LESSONS_EN: Lesson[] = [
  // --- Basic ---
  {
    title: '1. Writing',
    text: 'A computer can write anything for you. The \'print\' command shows text on the screen. Run it, then change the text to your own words!',
    code: 'print("Hello, world!")\nprint("I am learning Aksa")\n',
  },
  {
    title: '2. Variables',
    text: 'A variable is like a labeled box that stores something. Make the box with the keyword \'make\', then use its name anywhere. Try changing what is inside!',
    code: 'make name = "Aksa"\nmake age = 8\nprint(name, "is", age, "years old")\n',
  },
  {
    title: '3. Math',
    text: 'A computer is great at math. Use + to add, - to subtract, * to multiply, and / to divide. Try changing the numbers and watch the answers change!',
    code: 'make apples = 4\nmake oranges = 3\nprint("Total fruit:", apples + oranges)\nprint("Double the apples:", apples * 2)\nprint("One less orange:", oranges - 1)\n',
  },
  {
    title: '4. Asking',
    text: 'A program can ask you questions with the \'ask\' command. Your answer is stored in a variable. Run it, then type your answer in the black box below.',
    code: 'name = ask("What is your name?")\nprint("Hello,", name, "! Nice to meet you")\n',
  },
  {
    title: '5. Repeating',
    text: 'Tired of writing the same command again and again? Use \'repeat\'! Everything inside { } runs again. Try changing the number to 10.',
    code: 'repeat 3 {\n    print("Aksa is fun!")\n}\n',
  },
  {
    title: '6. Choosing',
    text: 'With \'if\', a program can choose. When the part inside ( ) is true it runs the first block; otherwise the \'else\' block. Try answering with different numbers!',
    code: 'age = ask("How old are you?")\nif (age >= 10) {\n    print("Wow, you are big!")\n} else {\n    print("You are small, but mighty!")\n}\n',
  },
  // --- Middle ---
  {
    title: '7. Counting',
    text: 'Want to know which loop you are on? Make a counter, then add 1 every time around. This is how you make a counting loop (like a "for" loop)!',
    code: 'make i = 1\nrepeat 5 {\n    print("Step", i)\n    i = i + 1\n}\n',
  },
  {
    title: '8. While',
    text: '\'while\' keeps repeating as long as its condition is true. This program counts down like a rocket. Careful: change the number inside, or it will never stop!',
    code: 'make number = 5\nwhile (number > 0) {\n    print(number)\n    number = number - 1\n}\nprint("Liftoff! 🚀")\n',
  },
  {
    title: '9. True & False',
    text: 'Something can only be true or false — nothing in between. You can store it in a variable and use it to choose. Join them with the keywords \'and\', \'or\', and \'not\'. Try changing \'true\' to \'false\'!',
    code: 'make had_breakfast = true\nmake is_raining = false\nif (had_breakfast and not is_raining) {\n    print("Let\'s play outside!")\n} else {\n    print("Let\'s play indoors instead")\n}\n',
  },
  {
    title: '10. Functions',
    text: 'A function is a command you invent yourself. Write the recipe once with the keyword \'function\', then call its name any time. Try making your own greeting!',
    code: 'function greet(name) {\n    print("Hello,", name, "!")\n}\ngreet("Aksa")\ngreet("Bima")\ngreet("Mom")\n',
  },
  {
    title: '11. Returning Answers',
    text: 'A function can also give back an answer with the keyword \'return\'. Call the function, then use its answer. Try making your own math function!',
    code: 'function area(side) {\n    return side * side\n}\nprint("Area of a 5 square:", area(5))\nprint("Area of a 10 square:", area(10))\n',
  },
  // --- Advanced ---
  {
    title: '12. Even or Odd',
    text: 'The % sign gives the remainder of a division. If a number divided by 2 leaves 0, it is even. This trick mixes counting and choosing!',
    code: 'make n = 1\nrepeat 10 {\n    if (n % 2 == 0) {\n        print(n, "is even")\n    } else {\n        print(n, "is odd")\n    }\n    n = n + 1\n}\n',
  },
  {
    title: '13. Robot',
    text: 'The robot draws while it walks. \'forward\' moves it, \'turn_right\' spins it. Four forwards and turns make a square. Try changing the numbers!',
    code: 'color("blue")\nrepeat 4 {\n    forward(100)\n    turn_right(90)\n}\n',
  },
  {
    title: '14. Project: Spiral',
    text: 'Project time! If every step is a little longer than the last, you get a spiral. Change the color, numbers, and turns — make a spiral that is truly yours!',
    code: 'make step = 5\ncolor("purple")\nrepeat 30 {\n    forward(step)\n    turn_right(25)\n    step = step + 3\n}\n',
  },
  {
    title: '15. Project: Guess the Number',
    text: 'The final project! The computer keeps a secret number. Keep guessing until you get it right. Change the secret, then challenge a friend to guess it!',
    code: 'make secret = 7\nmake guess = 0\nwhile (guess != secret) {\n    guess = ask("Guess a number from 1 to 10:")\n    if (guess < secret) {\n        print("Too small!")\n    } else if (guess > secret) {\n        print("Too big!")\n    }\n}\nprint("Great! The number was", secret)\n',
  },
];
