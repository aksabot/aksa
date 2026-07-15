#ifndef AKSA_EMITTER_H
#define AKSA_EMITTER_H

#include "error.h"
#include "locale.h"

/* Parse + check src, then emit readable C99 that mirrors the VM (core/vm.c).
   Returns a malloc'd C source string, or NULL if the front-end reported
   errors (which land in errs, exactly as the checker/VM would report them).
   locale_name is baked into aksa_rt_init(); srcname feeds #line directives. */
char *aksa_emit_c(const char *src, const AksaLocale *loc, const char *locale_name,
                  AksaErrors *errs, const char *srcname);

#endif
