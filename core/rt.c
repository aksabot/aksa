#include "rt.h"
#include "error.h"
#include "hal.h"
#include "lexer.h" /* TOK_TRUE / TOK_FALSE */
#include "locale.h"
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
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

#ifdef ARDUINO
#include "aksa_locale.h" /* locale JSON baked in at build time; no filesystem on device */

void aksa_rt_init(const char *locale) {
    (void)locale;
    char err[128];
    if (aksa_locale_load(&ak_loc, aksa_locale_json, err, sizeof err) == 0) {
        ak_true_word = kw_word(TOK_TRUE, "true");
        ak_false_word = kw_word(TOK_FALSE, "false");
    }
}
#else
#ifdef AK_TRACK
extern long ak_live;
static void ak_report_live(void) { fprintf(stderr, "AK_LIVE=%ld\n", ak_live); }
#endif
void aksa_rt_init(const char *locale) {
#ifdef AK_TRACK
    atexit(ak_report_live);
#endif
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
#endif

void aksa_rt_error(const char *id, int line, const char *arg) {
    AksaErrors e = {0};
    aksa_errors_add(&e, id, line, 0, arg);
    size_t cap = 128, len = 0;
    char *buf = malloc(cap);
    buf[0] = 0;
    aksa_errors_dump(&ak_loc, &e, &buf, &len, &cap);
    fputs(buf, stderr);
    free(buf);
#ifdef ARDUINO
    /* exit() reboots the chip and re-runs the program in a loop; halt instead
       so the message stays visible on the serial monitor. */
    fflush(stderr);
    for (;;) hal_wait(1000);
#else
    exit(1);
#endif
}

/* ---------- values ---------- */

Ak ak_nil(void) { return (Ak){AK_NIL, 0, NULL, 0}; }
Ak ak_num(double n) { return (Ak){AK_NUM, n, NULL, 0}; }
Ak ak_bool(int b) { return (Ak){AK_BOOL, b != 0, NULL, 0}; }
Ak ak_str(const char *s) { return (Ak){AK_STR, 0, s, 0}; }

double ak_asnum(Ak v) { return v.num; }
int ak_asbool(Ak v) { return v.num != 0; }

/* ---------- reference-counted heap strings ---------- */

/* A heap string is an AkStr header immediately before its bytes; Ak.str points
   at .s, and ak_retain/ak_release recover the header to touch the count. */
typedef struct {
    int rc;
    char s[];
} AkStr;

#ifdef AK_TRACK
long ak_live; /* live heap strings; a leak/double-free check for tests/mem.sh */
#endif

static AkStr *ak_hdr(Ak v) { return (AkStr *)(v.str - offsetof(AkStr, s)); }

/* build a heap string from the concatenation of a and b (b may be "") */
static Ak ak_heapstr(const char *a, const char *b) {
    size_t la = strlen(a), lb = strlen(b);
    AkStr *h = malloc(sizeof(AkStr) + la + lb + 1);
    h->rc = 1;
    memcpy(h->s, a, la);
    memcpy(h->s + la, b, lb + 1);
#ifdef AK_TRACK
    ak_live++;
#endif
    return (Ak){AK_STR, 0, h->s, 1};
}

Ak ak_retain(Ak v) {
    if (v.t == AK_STR && v.owned) ak_hdr(v)->rc++;
    return v;
}

void ak_release(Ak v) {
    if (v.t == AK_STR && v.owned) {
        AkStr *h = ak_hdr(v);
        if (--h->rc == 0) {
            free(h);
#ifdef AK_TRACK
            ak_live--;
#endif
        }
    }
}

Ak ak_assign(Ak *slot, Ak v) {
    Ak old = *slot;
    *slot = v;
    ak_release(old);
    return v;
}

int ak_truth(Ak v) {
    int b = ak_asbool(v);
    ak_release(v);
    return b;
}

double ak_count(Ak v) {
    double n = ak_asnum(v);
    ak_release(v);
    return n;
}

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

/* Every operator consumes (releases) its Ak arguments — each string value is
   produced fresh (a variable read is retained first, see the emitter), so the
   operator owns the one reference and must drop it. Read before releasing. */

Ak ak_add(Ak a, Ak b, int line) {
    if (a.t == AK_STR || b.t == AK_STR) {
        char ta[64], tb[64];
        const char *sa = ak_tostr(a, ta), *sb = ak_tostr(b, tb);
        Ak r = ak_heapstr(sa, sb);
        ak_release(a);
        ak_release(b);
        return r;
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
Ak ak_eq(Ak a, Ak b) {
    int r = ak_equal(a, b);
    ak_release(a);
    ak_release(b);
    return ak_bool(r);
}
Ak ak_neq(Ak a, Ak b) {
    int r = ak_equal(a, b);
    ak_release(a);
    ak_release(b);
    return ak_bool(!r);
}

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
        Ak v = va_arg(ap, Ak);
        fputs(ak_tostr(v, tmp), stdout);
        ak_release(v);
    }
    va_end(ap);
    fputs("\n", stdout);
    return ak_nil();
}

#ifdef ARDUINO
/* Localized word for a canonical builtin name (for error messages). */
static const char *builtin_word(const char *canon) {
    for (int i = 0; i < ak_loc.nbuiltins; i++)
        if (strcmp(ak_loc.builtins[i].canon, canon) == 0) return ak_loc.builtins[i].word;
    return canon;
}

Ak ak_ask(Ak prompt) {
    /* No keyboard on the device; asking a question can't work yet. */
    (void)prompt;
    aksa_rt_error("E106", 0, builtin_word("ask"));
    return ak_nil();
}
#else
Ak ak_ask(Ak prompt) {
    char tmp[64];
    if (prompt.t != AK_NIL) {
        fputs(ak_tostr(prompt, tmp), stdout);
        fputs(" ", stdout);
    }
    ak_release(prompt);
    char buf[256];
    if (!fgets(buf, sizeof buf, stdin)) buf[0] = 0;
    buf[strcspn(buf, "\r\n")] = 0;
    char *end;
    double d = strtod(buf, &end); /* numeric answers become numbers */
    if (end != buf && *end == 0) return ak_num(d);
    return ak_heapstr(buf, "");
}
#endif

Ak ak_pin_on(Ak pin) { hal_pin_on(ak_asnum(pin)); ak_release(pin); return ak_nil(); }
Ak ak_pin_off(Ak pin) { hal_pin_off(ak_asnum(pin)); ak_release(pin); return ak_nil(); }
Ak ak_pin_read(Ak pin) {
    double r = hal_pin_read(ak_asnum(pin));
    ak_release(pin);
    return ak_num(r);
}
Ak ak_pin_read_analog(Ak pin) {
    double r = hal_pin_read_analog(ak_asnum(pin));
    ak_release(pin);
    return ak_num(r);
}
Ak ak_wait(Ak sec) { hal_wait(ak_asnum(sec) * 1000.0); ak_release(sec); return ak_nil(); }
Ak ak_unsupported(const char *name, int line) {
    aksa_rt_error("E106", line, name);
    return ak_nil();
}
