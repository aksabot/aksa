#ifndef AKSA_PARSER_H
#define AKSA_PARSER_H

#include "ast.h"
#include "error.h"
#include "locale.h"

/* Parse a whole program. Always returns an AST_PROGRAM node (partial on
   errors); check errs->count. Panic-mode recovery: several errors per run. */
AksaNode *aksa_parse(const char *src, const AksaLocale *loc, AksaErrors *errs);

/* Parse and render: s-expression dump + localized errors. malloc'd. */
char *aksa_dump_ast(const char *src, const AksaLocale *loc, int *nerrors);

#endif
