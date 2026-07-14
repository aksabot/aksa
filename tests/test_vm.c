#include "test.h"
#include "../core/vm.h"
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

static char capture[4096];
static void capture_out(const char *text, void *user) {
    (void)user;
    strncat(capture, text, sizeof capture - strlen(capture) - 1);
}

/* run src, return captured output; err_id gets first error ID or "" */
static const char *run(const AksaLocale *loc, const char *src, char *err_id, int *err_line) {
    capture[0] = 0;
    AksaErrors errs = {0};
    aksa_run(src, loc, &errs, capture_out, NULL);
    if (err_id) strcpy(err_id, errs.count ? errs.items[0].id : "");
    if (err_line) *err_line = errs.count ? errs.items[0].line : 0;
    return capture;
}

int main(void) {
    load(&ID, "locales/id.json");
    load(&EN, "locales/en.json");
    char eid[8];
    int eline;

    /* hello + arithmetic */
    CHECK_STR(run(&ID, "tulis(\"halo\")", eid, NULL), "halo\n");
    CHECK_STR(eid, "");
    CHECK_STR(run(&ID, "tulis(1 + 2 * 3)", NULL, NULL), "7\n");
    CHECK_STR(run(&ID, "tulis((1 + 2) * 3)", NULL, NULL), "9\n");
    CHECK_STR(run(&ID, "tulis(7 % 3, 7 / 2, -2 + 1)", NULL, NULL), "1 3.5 -1\n");
    CHECK_STR(run(&ID, "tulis(\"umur: \" + 9)", NULL, NULL), "umur: 9\n");

    /* booleans print as localized words */
    CHECK_STR(run(&ID, "tulis(benar, salah)", NULL, NULL), "benar salah\n");
    CHECK_STR(run(&EN, "print(true, false)", NULL, NULL), "true false\n");
    CHECK_STR(run(&ID, "tulis(1 < 2, bukan benar, benar dan salah, salah atau benar)",
                  NULL, NULL), "benar salah salah benar\n");

    /* variables: globals, buat, strings */
    CHECK_STR(run(&ID, "x = 5\nx = x + 1\ntulis(x)", NULL, NULL), "6\n");
    CHECK_STR(run(&ID, "buat nama = \"Aksa\"\ntulis(\"halo \" + nama)", NULL, NULL),
              "halo Aksa\n");

    /* control flow */
    CHECK_STR(run(&ID, "jika (2 > 1) { tulis(\"ya\") } lainnya { tulis(\"bukan\") }",
                  NULL, NULL), "ya\n");
    CHECK_STR(run(&ID, "jika (1 > 2) { tulis(\"ya\") } lainnya { tulis(\"bukan\") }",
                  NULL, NULL), "bukan\n");
    CHECK_STR(run(&ID, "ulangi 3 { tulis(\"x\") }", NULL, NULL), "x\nx\nx\n");
    CHECK_STR(run(&ID, "i = 3\nselama (i > 0) { tulis(i)\ni = i - 1 }", NULL, NULL),
              "3\n2\n1\n");
    CHECK_STR(run(&ID, "i = 0\nselama (benar) { i = i + 1\njika (i == 3) { berhenti }\ntulis(i) }",
                  NULL, NULL), "1\n2\n");
    CHECK_STR(run(&ID, "ulangi 5 { berhenti }\ntulis(\"selesai\")", NULL, NULL), "selesai\n");
    /* nested loops: break only exits the inner one */
    CHECK_STR(run(&ID, "ulangi 2 { ulangi 5 { berhenti }\ntulis(\"a\") }", NULL, NULL),
              "a\na\n");

    /* functions: params, return, recursion, locals shadow globals */
    CHECK_STR(run(&ID, "fungsi tambah(a, b) { kembali a + b }\ntulis(tambah(2, 3))",
                  NULL, NULL), "5\n");
    CHECK_STR(run(&ID,
        "fungsi fib(n) { jika (n < 2) { kembali n }\nkembali fib(n-1) + fib(n-2) }\n"
        "tulis(fib(10))", eid, NULL), "55\n");
    CHECK_STR(eid, "");
    CHECK_STR(run(&ID,
        "x = 1\nfungsi f() { x = 99\nkembali x }\ntulis(f())\ntulis(x)",
        NULL, NULL), "99\n1\n");

    /* fizzbuzz-ish golden */
    CHECK_STR(run(&EN,
        "i = 1\nwhile (i <= 5) {\n"
        "  if (i % 3 == 0) { print(\"fizz\") } else { print(i) }\n"
        "  i = i + 1\n}", NULL, NULL), "1\n2\nfizz\n4\n5\n");

    /* runtime errors, with line numbers */
    run(&ID, "tulis(umur)", eid, &eline);
    CHECK_STR(eid, "E100");
    CHECK(eline == 1);
    run(&ID, "x = 1\ntulis(x / 0)", eid, &eline);
    CHECK_STR(eid, "E101");
    CHECK(eline == 2);
    run(&ID, "x = 5 % 0", eid, NULL);
    CHECK_STR(eid, "E101");
    run(&ID, "lompat(1)", eid, NULL);
    CHECK_STR(eid, "E102");
    run(&ID, "fungsi f(a) { kembali a }\ntulis(f(1, 2))", eid, &eline);
    CHECK_STR(eid, "E103");
    CHECK(eline == 2);
    run(&ID, "tulis(1 + benar)", eid, NULL);
    CHECK_STR(eid, "E104");
    run(&ID, "jika (1) { tulis(1) }", eid, NULL);
    CHECK_STR(eid, "E104"); /* conditions must be logika */
    run(&ID, "maju(10)", eid, NULL);
    CHECK_STR(eid, "E106"); /* turtle needs the browser (Phase 2) */
    /* undefined local read before assignment inside function */
    run(&ID, "fungsi f() { tulis(y)\ny = 1 }\nf()", eid, NULL);
    CHECK_STR(eid, "E100");

    /* compile-time misuse */
    run(&ID, "berhenti", eid, NULL);
    CHECK_STR(eid, "E107");
    run(&ID, "kembali 1", eid, NULL);
    CHECK_STR(eid, "E108");
    /* unbounded recursion stops with a kid-safe error, not a crash */
    run(&ID, "fungsi f() { kembali f() }\nf()", eid, NULL);
    CHECK_STR(eid, "E109");

    /* parse errors surface through aksa_run too, and nothing executes */
    CHECK_STR(run(&ID, "tulis(\"a\"\ntulis(\"b\")", eid, NULL), "");
    CHECK_STR(eid, "E005");

    return TEST_SUMMARY();
}
