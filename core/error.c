#include "error.h"
#include "locale.h"
#include <stdio.h>
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
