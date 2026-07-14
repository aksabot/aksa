#include "locale.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* hand-rolled reader for our fixed {"section":{"key":"value"}} JSON schema
   (strings only, two levels); swap in a real parser if the schema grows */

static const struct { const char *name; int tok; } TOKMAP[] = {
    {"IF", TOK_IF}, {"ELSE", TOK_ELSE}, {"REPEAT", TOK_REPEAT},
    {"WHILE", TOK_WHILE}, {"FUNCTION", TOK_FUNCTION}, {"RETURN", TOK_RETURN},
    {"TRUE", TOK_TRUE}, {"FALSE", TOK_FALSE}, {"AND", TOK_AND},
    {"OR", TOK_OR}, {"NOT", TOK_NOT}, {"VAR", TOK_VAR}, {"BREAK", TOK_BREAK},
};

typedef struct {
    const char *p;
    char *err;
    int errsz;
} P;

static int fail(P *ps, const char *msg) {
    snprintf(ps->err, ps->errsz, "locale JSON: %s", msg);
    return -1;
}

static void skipws(P *ps) {
    while (*ps->p == ' ' || *ps->p == '\t' || *ps->p == '\n' || *ps->p == '\r') ps->p++;
}

static int expect(P *ps, char c) {
    skipws(ps);
    if (*ps->p != c) {
        char msg[32];
        snprintf(msg, sizeof msg, "expected '%c'", c);
        return fail(ps, msg);
    }
    ps->p++;
    return 0;
}

static int hex4(const char *s) {
    int v = 0;
    for (int i = 0; i < 4; i++) {
        char c = s[i];
        v <<= 4;
        if (c >= '0' && c <= '9') v |= c - '0';
        else if (c >= 'a' && c <= 'f') v |= c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') v |= c - 'A' + 10;
        else return -1;
    }
    return v;
}

static int parse_string(P *ps, char *out, int outsz) {
    if (expect(ps, '"')) return -1;
    int o = 0;
    while (*ps->p && *ps->p != '"') {
        char c = *ps->p++;
        if (c == '\\') {
            char e = *ps->p;
            if (!e) break; /* unterminated, caught below */
            ps->p++;
            switch (e) {
            case '"': c = '"'; break;
            case '\\': c = '\\'; break;
            case '/': c = '/'; break;
            case 'n': c = '\n'; break;
            case 't': c = '\t'; break;
            case 'r': c = '\r'; break;
            case 'u': {
                int v = hex4(ps->p);
                if (v < 0) return fail(ps, "bad \\u escape");
                ps->p += 4;
                /* BMP only, no surrogate pairs — enough for locale files */
                if (v < 0x80) {
                    if (o >= outsz - 1) return fail(ps, "string too long");
                    out[o++] = (char)v;
                } else if (v < 0x800) {
                    if (o >= outsz - 2) return fail(ps, "string too long");
                    out[o++] = (char)(0xC0 | (v >> 6));
                    out[o++] = (char)(0x80 | (v & 0x3F));
                } else {
                    if (o >= outsz - 3) return fail(ps, "string too long");
                    out[o++] = (char)(0xE0 | (v >> 12));
                    out[o++] = (char)(0x80 | ((v >> 6) & 0x3F));
                    out[o++] = (char)(0x80 | (v & 0x3F));
                }
                continue;
            }
            default: return fail(ps, "bad escape");
            }
        }
        if (o >= outsz - 1) return fail(ps, "string too long");
        out[o++] = c;
    }
    if (*ps->p != '"') return fail(ps, "unterminated string");
    ps->p++;
    out[o] = 0;
    return 0;
}

static int add_entry(AksaLocale *loc, P *ps, const char *section, const char *key, const char *val) {
    if (strcmp(section, "keywords") == 0) {
        if (loc->nkeywords >= AKSA_MAX_KEYWORDS) return fail(ps, "too many keywords");
        for (size_t i = 0; i < sizeof TOKMAP / sizeof TOKMAP[0]; i++) {
            if (strcmp(TOKMAP[i].name, val) == 0) {
                snprintf(loc->keywords[loc->nkeywords].word, AKSA_WORD_MAX, "%s", key);
                loc->keywords[loc->nkeywords].tok = TOKMAP[i].tok;
                loc->nkeywords++;
                return 0;
            }
        }
        return fail(ps, "unknown token name in keywords");
    }
    if (strcmp(section, "builtins") == 0) {
        if (loc->nbuiltins >= AKSA_MAX_BUILTINS) return fail(ps, "too many builtins");
        snprintf(loc->builtins[loc->nbuiltins].word, AKSA_WORD_MAX, "%s", key);
        snprintf(loc->builtins[loc->nbuiltins].canon, AKSA_WORD_MAX, "%s", val);
        loc->nbuiltins++;
        return 0;
    }
    if (strcmp(section, "errors") == 0) {
        if (loc->nerrdefs >= AKSA_MAX_ERRDEFS) return fail(ps, "too many errors");
        snprintf(loc->errdefs[loc->nerrdefs].id, sizeof loc->errdefs[0].id, "%s", key);
        snprintf(loc->errdefs[loc->nerrdefs].msg, AKSA_MSG_MAX, "%s", val);
        loc->nerrdefs++;
        return 0;
    }
    return fail(ps, "unknown section");
}

int aksa_locale_load(AksaLocale *loc, const char *json, char *err, int errsz) {
    memset(loc, 0, sizeof *loc);
    if (errsz > 0) err[0] = 0;
    P ps = {json, err, errsz};
    if (expect(&ps, '{')) return -1;
    skipws(&ps);
    if (*ps.p == '}') return 0;
    for (;;) {
        char section[32];
        if (parse_string(&ps, section, sizeof section)) return -1;
        if (expect(&ps, ':')) return -1;
        if (expect(&ps, '{')) return -1;
        skipws(&ps);
        if (*ps.p == '}') {
            ps.p++;
        } else {
            for (;;) {
                char key[AKSA_WORD_MAX], val[AKSA_MSG_MAX];
                if (parse_string(&ps, key, sizeof key)) return -1;
                if (expect(&ps, ':')) return -1;
                if (parse_string(&ps, val, sizeof val)) return -1;
                if (add_entry(loc, &ps, section, key, val)) return -1;
                skipws(&ps);
                if (*ps.p == ',') { ps.p++; continue; }
                if (expect(&ps, '}')) return -1;
                break;
            }
        }
        skipws(&ps);
        if (*ps.p == ',') { ps.p++; continue; }
        return expect(&ps, '}');
    }
}

int aksa_locale_keyword(const AksaLocale *loc, const char *word, int len) {
    for (int i = 0; i < loc->nkeywords; i++)
        if ((int)strlen(loc->keywords[i].word) == len &&
            memcmp(loc->keywords[i].word, word, (size_t)len) == 0)
            return loc->keywords[i].tok;
    return -1;
}

const char *aksa_locale_builtin(const AksaLocale *loc, const char *word, int len) {
    for (int i = 0; i < loc->nbuiltins; i++)
        if ((int)strlen(loc->builtins[i].word) == len &&
            memcmp(loc->builtins[i].word, word, (size_t)len) == 0)
            return loc->builtins[i].canon;
    return NULL;
}

const char *aksa_locale_errmsg(const AksaLocale *loc, const char *id) {
    for (int i = 0; i < loc->nerrdefs; i++)
        if (strcmp(loc->errdefs[i].id, id) == 0) return loc->errdefs[i].msg;
    return NULL;
}

char *aksa_read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n < 0) { fclose(f); return NULL; }
    char *buf = malloc((size_t)n + 1);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, (size_t)n, f) != (size_t)n) { free(buf); fclose(f); return NULL; }
    fclose(f);
    buf[n] = 0;
    return buf;
}
