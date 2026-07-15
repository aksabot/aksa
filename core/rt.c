#include "rt.h"
#include "error.h"
#include "hal.h"
#include "lexer.h" /* TOK_TRUE / TOK_FALSE */
#include "locale.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- locale (loaded once for error text + boolean words) ---------- */

static AksaLocale ak_loc;
static const char *ak_true_word = "true", *ak_false_word = "false";

static const char *kw_word(int tok, const char *fallback) {
    for (int i = 0; i < ak_loc.nkeywords; i++)
        if (ak_loc.keywords[i].tok == tok) return ak_loc.keywords[i].word;
    return fallback;
}

void aksa_rt_init(const char *locale) {
    char path[512];
    snprintf(path, sizeof path, "locales/%s.json", locale);
    char *json = aksa_read_file(path);
    if (!json) return; /* messages fall back to English defaults */
    char err[128];
    if (aksa_locale_load(&ak_loc, json, err, sizeof err) == 0) {
        ak_true_word = kw_word(TOK_TRUE, "true");
        ak_false_word = kw_word(TOK_FALSE, "false");
    }
    free(json);
}

void aksa_rt_error(const char *id, int line, const char *arg) {
    AksaErrors e = {0};
    aksa_errors_add(&e, id, line, 0, arg);
    size_t cap = 128, len = 0;
    char *buf = malloc(cap);
    buf[0] = 0;
    aksa_errors_dump(&ak_loc, &e, &buf, &len, &cap);
    fputs(buf, stderr);
    free(buf);
    exit(1);
}

/* ---------- values ---------- */

Ak ak_nil(void) { return (Ak){AK_NIL, 0, NULL}; }
Ak ak_num(double n) { return (Ak){AK_NUM, n, NULL}; }
Ak ak_bool(int b) { return (Ak){AK_BOOL, b != 0, NULL}; }
Ak ak_str(const char *s) { return (Ak){AK_STR, 0, s}; }

double ak_asnum(Ak v) { return v.num; }
int ak_asbool(Ak v) { return v.num != 0; }

/* value → display text; tmp must hold 64 bytes (matches VM val_str) */
static const char *ak_tostr(Ak v, char *tmp) {
    switch (v.t) {
    case AK_NUM: snprintf(tmp, 64, "%.15g", v.num); return tmp;
    case AK_BOOL: return v.num ? ak_true_word : ak_false_word;
    case AK_STR: return v.str;
    default: return "";
    }
}

static int both_num(Ak a, Ak b) { return a.t == AK_NUM && b.t == AK_NUM; }

Ak ak_add(Ak a, Ak b, int line) {
    if (a.t == AK_STR || b.t == AK_STR) {
        char ta[64], tb[64];
        const char *sa = ak_tostr(a, ta), *sb = ak_tostr(b, tb);
        /* Concatenation strings are never freed: emitted programs are
           short-lived on the desktop. Device loops run forever, so P5 needs an
           arena or explicit free here. */
        char *s = malloc(strlen(sa) + strlen(sb) + 1);
        strcpy(s, sa);
        strcat(s, sb);
        return ak_str(s);
    }
    if (!both_num(a, b)) aksa_rt_error("E104", line, "");
    return ak_num(a.num + b.num);
}
Ak ak_sub(Ak a, Ak b, int line) {
    if (!both_num(a, b)) aksa_rt_error("E104", line, "");
    return ak_num(a.num - b.num);
}
Ak ak_mul(Ak a, Ak b, int line) {
    if (!both_num(a, b)) aksa_rt_error("E104", line, "");
    return ak_num(a.num * b.num);
}
Ak ak_div(Ak a, Ak b, int line) {
    if (!both_num(a, b)) aksa_rt_error("E104", line, "");
    if (b.num == 0) aksa_rt_error("E101", line, "");
    return ak_num(a.num / b.num);
}
Ak ak_mod(Ak a, Ak b, int line) {
    if (!both_num(a, b)) aksa_rt_error("E104", line, "");
    if (b.num == 0) aksa_rt_error("E101", line, "");
    return ak_num(fmod(a.num, b.num));
}
Ak ak_lt(Ak a, Ak b, int line) {
    if (!both_num(a, b)) aksa_rt_error("E104", line, "");
    return ak_bool(a.num < b.num);
}
Ak ak_gt(Ak a, Ak b, int line) {
    if (!both_num(a, b)) aksa_rt_error("E104", line, "");
    return ak_bool(a.num > b.num);
}
Ak ak_le(Ak a, Ak b, int line) {
    if (!both_num(a, b)) aksa_rt_error("E104", line, "");
    return ak_bool(a.num <= b.num);
}
Ak ak_ge(Ak a, Ak b, int line) {
    if (!both_num(a, b)) aksa_rt_error("E104", line, "");
    return ak_bool(a.num >= b.num);
}

static int ak_equal(Ak a, Ak b) {
    if (a.t != b.t) return 0;
    switch (a.t) {
    case AK_NUM: return a.num == b.num;
    case AK_BOOL: return (a.num != 0) == (b.num != 0);
    case AK_STR: return strcmp(a.str, b.str) == 0;
    default: return 1;
    }
}
Ak ak_eq(Ak a, Ak b) { return ak_bool(ak_equal(a, b)); }
Ak ak_neq(Ak a, Ak b) { return ak_bool(!ak_equal(a, b)); }

Ak ak_not(Ak a, int line) {
    if (a.t != AK_BOOL) aksa_rt_error("E104", line, "");
    return ak_bool(!(a.num != 0));
}
Ak ak_neg(Ak a, int line) {
    if (a.t != AK_NUM) aksa_rt_error("E104", line, "");
    return ak_num(-a.num);
}

/* ---------- builtins ---------- */

Ak ak_print(int n, ...) {
    va_list ap;
    va_start(ap, n);
    char tmp[64];
    for (int i = 0; i < n; i++) {
        if (i) fputs(" ", stdout);
        fputs(ak_tostr(va_arg(ap, Ak), tmp), stdout);
    }
    va_end(ap);
    fputs("\n", stdout);
    return ak_nil();
}

Ak ak_ask(Ak prompt) {
    char tmp[64];
    if (prompt.t != AK_NIL) {
        fputs(ak_tostr(prompt, tmp), stdout);
        fputs(" ", stdout);
    }
    char buf[256];
    if (!fgets(buf, sizeof buf, stdin)) buf[0] = 0;
    buf[strcspn(buf, "\r\n")] = 0;
    char *end;
    double d = strtod(buf, &end); /* numeric answers become numbers */
    if (end != buf && *end == 0) return ak_num(d);
    char *s = malloc(strlen(buf) + 1); /* never freed, see ak_add */
    strcpy(s, buf);
    return ak_str(s);
}

Ak ak_pin_on(Ak pin) { hal_pin_on(ak_asnum(pin)); return ak_nil(); }
Ak ak_pin_off(Ak pin) { hal_pin_off(ak_asnum(pin)); return ak_nil(); }
Ak ak_pin_read(Ak pin) { return ak_num(hal_pin_read(ak_asnum(pin))); }
Ak ak_pin_read_analog(Ak pin) { return ak_num(hal_pin_read_analog(ak_asnum(pin))); }
Ak ak_wait(Ak ms) { hal_wait(ak_asnum(ms)); return ak_nil(); }
Ak ak_unsupported(const char *name, int line) {
    aksa_rt_error("E106", line, name);
    return ak_nil();
}
