#ifndef AKSA_ERROR_H
#define AKSA_ERROR_H

#define AKSA_MAX_ERRORS 32

typedef struct {
    const char *id; /* stable error ID, e.g. "E001" (points to a static string) */
    int line, col;
    char arg[64];   /* offending text; fills {char}/{text} in locale templates */
} AksaError;

typedef struct {
    AksaError items[AKSA_MAX_ERRORS];
    int count;
} AksaErrors;

void aksa_errors_add(AksaErrors *e, const char *id, int line, int col, const char *arg);

struct AksaLocale;
void aksa_error_format(const struct AksaLocale *loc, const AksaError *e, char *out, int outsz);

#endif
