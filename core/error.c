#include "error.h"
#include "locale.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void aksa_errors_add(AksaErrors *e, const char *id, int line, int col, const char *arg) {
    if (e->count >= AKSA_MAX_ERRORS) return;
    AksaError *it = &e->items[e->count++];
    it->id = id;
    it->line = line;
    it->col = col;
    snprintf(it->arg, sizeof it->arg, "%s", arg ? arg : "");
}

/* Fill a locale template: {line}/{col} from the error position, any other
   {placeholder} ({char}, {text}, ...) from the recorded offending text. */
void aksa_error_format(const struct AksaLocale *loc, const AksaError *e, char *out, int outsz) {
    const char *tpl = aksa_locale_errmsg(loc, e->id);
    if (!tpl) {
        snprintf(out, outsz, "%s (line %d)", e->id, e->line);
        return;
    }
    int o = 0;
    const char *p = tpl;
    while (*p && o < outsz - 1) {
        const char *end;
        if (*p == '{' && (end = strchr(p, '}')) != NULL) {
            int n = (int)(end - p - 1);
            if (n == 4 && strncmp(p + 1, "line", 4) == 0)
                o += snprintf(out + o, outsz - o, "%d", e->line);
            else if (n == 3 && strncmp(p + 1, "col", 3) == 0)
                o += snprintf(out + o, outsz - o, "%d", e->col);
            else
                o += snprintf(out + o, outsz - o, "%s", e->arg);
            if (o > outsz - 1) o = outsz - 1;
            p = end + 1;
            continue;
        }
        out[o++] = *p++;
    }
    out[o] = 0;
}

void aksa_sb_put(char **buf, size_t *len, size_t *cap, const char *s) {
    size_t n = strlen(s);
    if (*len + n + 1 > *cap) {
        while (*len + n + 1 > *cap) *cap *= 2;
        *buf = realloc(*buf, *cap);
    }
    memcpy(*buf + *len, s, n + 1);
    *len += n;
}

void aksa_errors_dump(const struct AksaLocale *loc, const AksaErrors *e,
                      char **buf, size_t *len, size_t *cap) {
    for (int i = 0; i < e->count; i++) {
        char msg[256], lineb[320];
        aksa_error_format(loc, &e->items[i], msg, sizeof msg);
        snprintf(lineb, sizeof lineb, "! %s %d:%d %s\n", e->items[i].id,
                 e->items[i].line, e->items[i].col, msg);
        aksa_sb_put(buf, len, cap, lineb);
    }
}
