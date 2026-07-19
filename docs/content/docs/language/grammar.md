---
title: Grammar
description: The formal grammar of Aksa, for the curious.
---

Aksa's grammar is small enough to read in one sitting. Keywords below are abstract — each locale supplies its own words for them.

```
program     := statement*
statement   := if_stmt | repeat_stmt | while_stmt | func_def
             | return_stmt | break_stmt | assign_stmt | expr_stmt
if_stmt     := IF "(" expr ")" block (ELSE (if_stmt | block))?
repeat_stmt := REPEAT expr block            // repeat 10 { ... }
while_stmt  := WHILE "(" expr ")" block
func_def    := FUNCTION IDENT "(" params? ")" block
assign_stmt := (VAR)? IDENT "=" expr
block       := "{" statement* "}"
expr        := standard precedence: or → and → not → comparison
               (==, !=, <, >, <=, >=) → add/sub → mul/div/mod → unary → call/primary
```

## Notes

- Braces and parentheses are C-like on purpose — Aksa graduates to real C on hardware, and the shapes should feel familiar.
- `VAR` (`make` / `buat`) is optional: assigning to a new name creates it.
- Identifiers allow full Unicode letters, so variables and functions can be named in any language.
- Comments run from `//` to the end of the line.
- The parser is recursive descent with panic-mode recovery: it reports several errors per run instead of stopping at the first one.
