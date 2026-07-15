#include "../core/checker.h"
#include "../core/lexer.h"
#include "../core/parser.h"
#include "../core/vm.h"
#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Page-installed hooks. The async ones suspend the wasm run until the JS
   promise resolves (input typed, timeout fired), keeping the tab alive. */
EM_JS(void, js_out, (const char *p), { Module.aksaOut(UTF8ToString(p)); })
EM_ASYNC_JS(void, js_in, (char *buf, int sz),
            { stringToUTF8(await Module.aksaInput(), buf, sz); })
EM_ASYNC_JS(int, js_yield, (void), { return await Module.aksaYield(); })
EM_ASYNC_JS(int, js_host, (const char *canon, double num, const char *str), {
    return await Module.aksaHost(UTF8ToString(canon), num,
                                 str ? UTF8ToString(str) : null) ? 1 : 0;
})

static void hook_out(const char *text, void *user) {
    (void)user;
    js_out(text);
}
static void hook_in(char *buf, int bufsz, void *user) {
    (void)user;
    js_in(buf, bufsz);
}
static int hook_yield(void *user) {
    (void)user;
    return js_yield();
}
static int hook_host(const char *canon, double num, const char *str, void *user) {
    (void)user;
    return js_host(canon, num, str);
}

static void json_escape(char **buf, size_t *len, size_t *cap, const char *s) {
    char one[8];
    for (; *s; s++) {
        if (*s == '"' || *s == '\\')
            snprintf(one, sizeof one, "\\%c", *s);
        else if ((unsigned char)*s < 0x20)
            snprintf(one, sizeof one, "\\u%04x", *s);
        else
            snprintf(one, sizeof one, "%c", *s);
        aksa_sb_put(buf, len, cap, one);
    }
}

/* errors → malloc'd JSON: [{"id":"E101","line":3,"col":0,"msg":"..."}] */
static char *errors_json(const AksaLocale *loc, const AksaErrors *errs) {
    size_t cap = 64, len = 0;
    char *buf = malloc(cap);
    buf[0] = 0;
    aksa_sb_put(&buf, &len, &cap, "[");
    for (int i = 0; i < errs->count; i++) {
        const AksaError *e = &errs->items[i];
        char msg[256], head[64];
        aksa_error_format(loc, e, msg, sizeof msg);
        snprintf(head, sizeof head, "%s{\"id\":\"%s\",\"line\":%d,\"col\":%d,\"msg\":\"",
                 i ? "," : "", e->id, e->line, e->col);
        aksa_sb_put(&buf, &len, &cap, head);
        json_escape(&buf, &len, &cap, msg);
        aksa_sb_put(&buf, &len, &cap, "\"}");
    }
    aksa_sb_put(&buf, &len, &cap, "]");
    return buf;
}

static char *locale_error(const char *err) {
    char *out = malloc(strlen(err) + 64);
    sprintf(out, "[{\"id\":\"\",\"line\":0,\"col\":0,\"msg\":\"%s\"}]", err);
    return out;
}

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

/* Run `src`; output/input/yield go through the Module.aksa* hooks.
   Returns malloc'd error JSON ("[]" on success); JS frees with _free(). */
EMSCRIPTEN_KEEPALIVE
char *aksa_wasm_run(const char *src, const char *locale_json) {
    AksaLocale loc;
    char err[128];
    if (aksa_locale_load(&loc, locale_json, err, sizeof err) != 0)
        return locale_error(err);
    AksaErrors errs = {0};
    AksaHost host = {.out = hook_out, .in = hook_in, .yield = hook_yield,
                     .host = hook_host, .user = NULL};
    aksa_run(src, &loc, &errs, &host);
    return errors_json(&loc, &errs);
}

/* Parse + check only (no run); same JSON shape. Powers live squiggles. */
EMSCRIPTEN_KEEPALIVE
char *aksa_wasm_check(const char *src, const char *locale_json) {
    AksaLocale loc;
    char err[128];
    if (aksa_locale_load(&loc, locale_json, err, sizeof err) != 0)
        return locale_error(err);
    AksaErrors errs = {0};
    AksaNode *prog = aksa_parse(src, &loc, &errs);
    if (errs.count == 0) aksa_check(prog, &loc, &errs);
    aksa_ast_free(prog);
    return errors_json(&loc, &errs);
}
