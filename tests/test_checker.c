#include "test.h"
#include "../core/checker.h"
#include "../core/parser.h"
#include "../core/lexer.h"
#include <stdlib.h>

static AksaLocale ID, EN;

static void load(AksaLocale *loc, const char *path) {
    char err[128];
    char *json = aksa_read_file(path);
    if (!json || aksa_locale_load(loc, json, err, sizeof err) != 0) {
        printf("FATAL: cannot load %s\n", path);
        exit(1);
    }
    free(json);
}

/* returns error count; ids/lines filled for the first `max` errors */
static int check(const AksaLocale *loc, const char *src, const char *ids[], int max) {
    AksaErrors errs = {0};
    AksaNode *prog = aksa_parse(src, loc, &errs);
    int parse_errs = errs.count;
    if (!parse_errs) aksa_check(prog, loc, &errs);
    aksa_ast_free(prog);
    for (int i = 0; i < max; i++) ids[i] = i < errs.count ? errs.items[i].id : "";
    (void)parse_errs;
    return errs.count;
}

int main(void) {
    load(&ID, "locales/id.json");
    load(&EN, "locales/en.json");
    const char *ids[8];

    /* clean programs stay clean */
    CHECK(check(&ID, "tulis(\"halo\")", ids, 0) == 0);
    CHECK(check(&ID, "x = 5\ntulis(x + 1)", ids, 0) == 0);
    CHECK(check(&ID,
        "fungsi fib(n) { jika (n < 2) { kembali n }\nkembali fib(n-1) + fib(n-2) }\n"
        "tulis(fib(10))", ids, 0) == 0);
    /* turtle/hardware builtins are valid statically (E106 is runtime-only) */
    CHECK(check(&ID, "ulangi 4 { maju(50)\nbelok_kanan(90) }", ids, 0) == 0);
    /* main-loop idiom: function reads a global assigned later at top level */
    CHECK(check(&ID, "fungsi f() { tulis(skor) }\nskor = 0\nf()", ids, 0) == 0);
    /* unknown-typed conditions (variables, calls, ask) are trusted */
    CHECK(check(&ID, "x = tanya()\njika (x == 1) { }\njika (baca(2) > 0) { }", ids, 0) == 0);

    /* E100: use before any assignment */
    CHECK(check(&ID, "tulis(umur)", ids, 1) == 1);
    CHECK_STR(ids[0], "E100");
    CHECK(check(&ID, "tulis(x)\nx = 1", ids, 1) == 1); /* order matters */
    CHECK_STR(ids[0], "E100");
    /* local shadows global: read before assignment inside the function */
    CHECK(check(&ID, "x = 1\nfungsi f() { tulis(x)\nx = 2 }\nf()", ids, 1) == 1);
    CHECK_STR(ids[0], "E100");

    /* E102: unknown command (typo) */
    CHECK(check(&ID, "tuls(1)", ids, 1) == 1);
    CHECK_STR(ids[0], "E102");

    /* E103: wrong arity — user functions and builtins */
    CHECK(check(&ID, "fungsi f(a) { kembali a }\nf(1, 2)", ids, 1) == 1);
    CHECK_STR(ids[0], "E103");
    CHECK(check(&ID, "maju()", ids, 1) == 1);
    CHECK_STR(ids[0], "E103");
    CHECK(check(&ID, "maju(1, 2)", ids, 1) == 1);
    CHECK_STR(ids[0], "E103");
    CHECK(check(&ID, "x = tanya(\"a\", \"b\")", ids, 1) == 1);
    CHECK_STR(ids[0], "E103");
    CHECK(check(&ID, "tulis(1, 2, 3)", ids, 0) == 0); /* tulis is variadic */

    /* E104: operand type mismatch */
    CHECK(check(&ID, "x = 1 - benar", ids, 1) == 1);
    CHECK_STR(ids[0], "E104");
    CHECK(check(&ID, "x = 1 < \"a\"", ids, 1) == 1);
    CHECK_STR(ids[0], "E104");
    CHECK(check(&ID, "x = benar dan 1", ids, 1) == 1);
    CHECK_STR(ids[0], "E104");
    CHECK(check(&ID, "x = \"skor: \" + 3", ids, 0) == 0); /* concat is fine */

    /* E105: condition must be boolean */
    CHECK(check(&ID, "jika (5) { }", ids, 1) == 1);
    CHECK_STR(ids[0], "E105");
    CHECK(check(&ID, "selama (\"x\") { }", ids, 1) == 1);
    CHECK_STR(ids[0], "E105");
    CHECK(check(&ID, "jika (1 + 2) { }", ids, 1) == 1);
    CHECK_STR(ids[0], "E105");

    /* E110: repeat needs a number */
    CHECK(check(&ID, "ulangi benar { }", ids, 1) == 1);
    CHECK_STR(ids[0], "E110");
    CHECK(check(&ID, "ulangi \"3\" { }", ids, 1) == 1);
    CHECK_STR(ids[0], "E110");

    /* E107/E108: berhenti/kembali in the wrong place */
    CHECK(check(&ID, "berhenti", ids, 1) == 1);
    CHECK_STR(ids[0], "E107");
    CHECK(check(&ID, "kembali 1", ids, 1) == 1);
    CHECK_STR(ids[0], "E108");
    /* berhenti inside a function doesn't see the loop outside it */
    CHECK(check(&ID, "ulangi 3 { fungsi f() { berhenti }\nf() }", ids, 1) == 1);
    CHECK_STR(ids[0], "E107");

    /* E111: redefinition */
    CHECK(check(&ID, "fungsi f() { }\nfungsi f() { }", ids, 1) == 1);
    CHECK_STR(ids[0], "E111");
    CHECK(check(&ID, "fungsi tulis() { }", ids, 1) == 1); /* clashes with builtin */
    CHECK_STR(ids[0], "E111");
    CHECK(check(&ID, "fungsi f(a, a) { }", ids, 1) == 1); /* duplicate param */
    CHECK_STR(ids[0], "E111");

    /* several problems reported in one pass */
    CHECK(check(&ID, "tulis(umur)\njika (5) { }\nmaju()", ids, 3) == 3);
    CHECK_STR(ids[0], "E100");
    CHECK_STR(ids[1], "E105");
    CHECK_STR(ids[2], "E103");

    /* en locale behaves identically */
    CHECK(check(&EN, "print(age)", ids, 1) == 1);
    CHECK_STR(ids[0], "E100");
    CHECK(check(&EN, "if (5) { }", ids, 1) == 1);
    CHECK_STR(ids[0], "E105");

    return TEST_SUMMARY();
}
