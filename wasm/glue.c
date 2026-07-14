#include "../core/lexer.h"
#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tokenize `src` with the given locale JSON; returns a malloc'd string the
   caller (JS) must free with _free(). */
EMSCRIPTEN_KEEPALIVE
char *aksa_wasm_tokens(const char *src, const char *locale_json) {
    AksaLocale loc;
    char err[128];
    if (aksa_locale_load(&loc, locale_json, err, sizeof err) != 0) {
        char *out = malloc(strlen(err) + 2);
        sprintf(out, "%s\n", err);
        return out;
    }
    return aksa_dump_tokens(src, &loc, NULL);
}
