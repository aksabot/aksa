#ifndef AKSA_CHECKER_H
#define AKSA_CHECKER_H

#include "ast.h"
#include "error.h"
#include "locale.h"

/* Semantic analysis on the AST. Reports (localized IDs, with positions):
   E100 use of an undefined variable        E107 'berhenti' outside a loop
   E102 unknown function/builtin            E108 'kembali' outside a function
   E103 wrong number of arguments           E110 non-number 'ulangi' count
   E104 operand type mismatch               E111 redefinition
   E105 non-boolean condition
   Never a false alarm: unknown types (variables, call results) pass. */
void aksa_check(const AksaNode *prog, const AksaLocale *loc, AksaErrors *errs);

#endif
