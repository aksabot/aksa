#ifndef AKSA_VM_H
#define AKSA_VM_H

#include "error.h"
#include "locale.h"

typedef void (*AksaOutFn)(const char *text, void *user);
typedef void (*AksaInFn)(char *buf, int bufsz, void *user); /* read one line */
typedef int (*AksaYieldFn)(void *user);                     /* nonzero => stop */
/* Turtle/hardware builtins: canonical name + its single number or string
   argument (builtins take at most one, checker-enforced). Reads (pin_read,
   pin_read_analog) write their value into *result; other builtins leave it
   alone. Return 1 if handled, 0 if unknown (surfaces the "not available
   here" error). */
typedef int (*AksaHostFn)(const char *canon, double num, const char *str,
                          double *result, void *user);

/* Host hooks for embedding; any NULL member falls back to the default. */
typedef struct {
    AksaOutFn out;     /* NULL: stdout */
    AksaInFn in;       /* NULL: fgets(stdin) */
    AksaYieldFn yield; /* NULL: never yields */
    AksaHostFn host;   /* NULL: turtle/hardware builtins error */
    void *user;
} AksaHost;

/* Parse, compile to bytecode, and run. Returns 0 on success; all problems
   (parse, compile, runtime) land in errs as localized error IDs.
   host == NULL uses all defaults (native CLI path). */
int aksa_run(const char *src, const AksaLocale *loc, AksaErrors *errs,
             const AksaHost *host);

#endif
