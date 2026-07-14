#ifndef AKSA_LOCALE_H
#define AKSA_LOCALE_H

#define AKSA_MAX_KEYWORDS 24
#define AKSA_MAX_BUILTINS 32
#define AKSA_MAX_ERRDEFS 64
#define AKSA_WORD_MAX 48
#define AKSA_MSG_MAX 160

typedef struct AksaLocale {
    struct { char word[AKSA_WORD_MAX]; int tok; } keywords[AKSA_MAX_KEYWORDS];
    int nkeywords;
    struct { char word[AKSA_WORD_MAX]; char canon[AKSA_WORD_MAX]; } builtins[AKSA_MAX_BUILTINS];
    int nbuiltins;
    struct { char id[8]; char msg[AKSA_MSG_MAX]; } errdefs[AKSA_MAX_ERRDEFS];
    int nerrdefs;
} AksaLocale;

/* Load a locale from JSON text. 0 on success; on failure writes a reason to err. */
int aksa_locale_load(AksaLocale *loc, const char *json, char *err, int errsz);

/* Token kind for a keyword, or -1 if `word` is not a keyword in this locale. */
int aksa_locale_keyword(const AksaLocale *loc, const char *word, int len);

/* Canonical builtin name, or NULL if not a builtin in this locale. */
const char *aksa_locale_builtin(const AksaLocale *loc, const char *word, int len);

/* Localized message template for an error ID, or NULL. */
const char *aksa_locale_errmsg(const AksaLocale *loc, const char *id);

/* Whole file as a malloc'd NUL-terminated string, or NULL. */
char *aksa_read_file(const char *path);

#endif
