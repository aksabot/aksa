#ifndef AKSA_VM_H
#define AKSA_VM_H

#include "error.h"
#include "locale.h"

typedef void (*AksaOutFn)(const char *text, void *user);

/* Parse, compile to bytecode, and run. Returns 0 on success; all problems
   (parse, compile, runtime) land in errs as localized error IDs.
   out == NULL prints to stdout. */
int aksa_run(const char *src, const AksaLocale *loc, AksaErrors *errs,
             AksaOutFn out, void *user);

#endif
