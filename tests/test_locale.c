#include "test.h"
#include "../core/lexer.h"
#include <stdlib.h>

int main(void) {
    char err[128];
    AksaLocale loc;

    /* real locale files load and resolve */
    const char *files[2] = {"locales/id.json", "locales/en.json"};
    for (int i = 0; i < 2; i++) {
        char *json = aksa_read_file(files[i]);
        CHECK(json != NULL);
        if (!json) continue;
        CHECK(aksa_locale_load(&loc, json, err, sizeof err) == 0);
        CHECK(loc.nkeywords == 13);
        CHECK(loc.nbuiltins == 14);
        CHECK(loc.nerrdefs >= 3);
        CHECK(aksa_locale_errmsg(&loc, "E001") != NULL);
        CHECK(aksa_locale_errmsg(&loc, "E999") == NULL);
        free(json);
    }

    /* keyword + builtin lookup, Indonesian */
    char *json = aksa_read_file("locales/id.json");
    CHECK(json != NULL);
    CHECK(aksa_locale_load(&loc, json, err, sizeof err) == 0);
    free(json);
    CHECK(aksa_locale_keyword(&loc, "jika", 4) == TOK_IF);
    CHECK(aksa_locale_keyword(&loc, "berhenti", 8) == TOK_BREAK);
    CHECK(aksa_locale_keyword(&loc, "jik", 3) == -1);
    CHECK(aksa_locale_keyword(&loc, "if", 2) == -1); /* en keyword absent in id */
    CHECK_STR(aksa_locale_builtin(&loc, "tulis", 5), "print");
    CHECK(aksa_locale_builtin(&loc, "print", 5) == NULL);

    /* escapes: \n, \", \uXXXX (é = U+00E9) */
    CHECK(aksa_locale_load(&loc,
        "{\"errors\":{\"E900\":\"a\\nb \\\"q\\\" caf\\u00e9\"}}", err, sizeof err) == 0);
    CHECK_STR(aksa_locale_errmsg(&loc, "E900"), "a\nb \"q\" caf\xc3\xa9");

    /* bad inputs rejected with a reason */
    CHECK(aksa_locale_load(&loc, "{\"keywords\":{\"jika\":\"BOGUS\"}}", err, sizeof err) == -1);
    CHECK(err[0] != 0);
    CHECK(aksa_locale_load(&loc, "{\"nosuchsection\":{\"a\":\"b\"}}", err, sizeof err) == -1);
    CHECK(aksa_locale_load(&loc, "{\"errors\":{\"E1\":\"oops", err, sizeof err) == -1);
    CHECK(aksa_locale_load(&loc, "not json", err, sizeof err) == -1);

    /* empty object is fine */
    CHECK(aksa_locale_load(&loc, "{}", err, sizeof err) == 0);
    CHECK(loc.nkeywords == 0);

    return TEST_SUMMARY();
}
