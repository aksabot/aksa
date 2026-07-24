#include "test.h"
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

/* parse with locale, compare s-expression dump; returns error count */
static int check_ast(const AksaLocale *loc, const char *src, const char *want) {
    AksaErrors errs = {0};
    AksaNode *prog = aksa_parse(src, loc, &errs);
    char *got = aksa_ast_to_string(prog);
    CHECK_STR(got, want);
    free(got);
    aksa_ast_free(prog);
    return errs.count;
}

static const char *err_id(const AksaLocale *loc, const char *src, int idx, int *count,
                          char *arg, int argsz) {
    AksaErrors errs = {0};
    AksaNode *prog = aksa_parse(src, loc, &errs);
    aksa_ast_free(prog);
    *count = errs.count;
    if (idx >= errs.count) return "";
    if (arg) snprintf(arg, (size_t)argsz, "%s", errs.items[idx].arg);
    return errs.items[idx].id;
}

int main(void) {
    load(&ID, "locales/id.json");
    load(&EN, "locales/en.json");
    int n;
    char arg[64];

    /* literals, calls, precedence */
    CHECK(check_ast(&ID, "cetak(\"halo\")", "(call cetak (str \"halo\"))") == 0);
    CHECK(check_ast(&ID, "x = 1 + 2 * 3", "(assign x (+ 1 (* 2 3)))") == 0);
    CHECK(check_ast(&ID, "x = (1 + 2) * 3", "(assign x (* (+ 1 2) 3))") == 0);
    CHECK(check_ast(&ID, "x = -2 + 3", "(assign x (+ (neg 2) 3))") == 0);
    CHECK(check_ast(&ID, "x = 10 % 3 - 1", "(assign x (- (% 10 3) 1))") == 0);
    CHECK(check_ast(&ID, "buat umur = 9", "(decl umur 9)") == 0);

    /* booleans and logic: or → and → not → comparison */
    CHECK(check_ast(&ID, "x = bukan benar dan salah", "(assign x (and (not true) false))") == 0);
    CHECK(check_ast(&ID, "x = 1 < 2 atau 3 >= 4", "(assign x (or (< 1 2) (>= 3 4)))") == 0);
    CHECK(check_ast(&ID, "x = bukan (1 == 2)", "(assign x (not (== 1 2)))") == 0);

    /* control flow */
    CHECK(check_ast(&ID, "jika (benar) { cetak(1) } lainnya { cetak(2) }",
                    "(if true (block (call cetak 1)) (block (call cetak 2)))") == 0);
    CHECK(check_ast(&ID, "jika (x > 1) { } lainnya jika (x < 0) { }",
                    "(if (> x 1) (block) (if (< x 0) (block)))") == 0);
    CHECK(check_ast(&ID, "ulangi 10 { maju(50) }", "(repeat 10 (block (call maju 50)))") == 0);
    CHECK(check_ast(&ID, "selama (benar) { berhenti }", "(while true (block (break)))") == 0);

    /* functions */
    CHECK(check_ast(&ID, "fungsi tambah(a, b) { kembali a + b }",
                    "(func tambah (params a b) (block (return (+ a b))))") == 0);
    CHECK(check_ast(&ID, "fungsi f() { kembali }",
                    "(func f (params) (block (return)))") == 0);

    /* multiple top-level statements */
    CHECK(check_ast(&ID, "x = 1\ncetak(x)", "(assign x 1)\n(call cetak x)") == 0);

    /* string escapes are decoded into the AST */
    CHECK(check_ast(&ID, "cetak(\"a\\nb\")", "(call cetak (str \"a\nb\"))") == 0);

    /* locale invariance: same structure from en source */
    CHECK(check_ast(&EN, "function add(a, b) { return a + b }",
                    "(func add (params a b) (block (return (+ a b))))") == 0);
    CHECK(check_ast(&EN, "if (true) { print(1) } else { print(2) }",
                    "(if true (block (call print 1)) (block (call print 2)))") == 0);

    /* node positions survive into the AST */
    {
        AksaErrors errs = {0};
        AksaNode *prog = aksa_parse("\n  x = 5", &ID, &errs);
        CHECK(prog->nlist == 1);
        CHECK(prog->list[0]->kind == AST_ASSIGN);
        CHECK(prog->list[0]->line == 2 && prog->list[0]->col == 3);
        aksa_ast_free(prog);
    }

    /* errors: missing ')' */
    CHECK_STR(err_id(&ID, "cetak(1", 0, &n, arg, sizeof arg), "E005");
    CHECK_STR(arg, ")");
    /* missing '}' */
    CHECK_STR(err_id(&ID, "jika (benar) { cetak(1)", 0, &n, arg, sizeof arg), "E005");
    CHECK_STR(arg, "}");
    /* missing '=' after buat name */
    CHECK_STR(err_id(&ID, "buat x 5", 0, &n, arg, sizeof arg), "E005");
    CHECK_STR(arg, "=");
    /* unexpected token */
    CHECK_STR(err_id(&ID, "x = ", 0, &n, arg, sizeof arg), "E004");
    CHECK_STR(err_id(&ID, "}", 0, &n, arg, sizeof arg), "E004");
    CHECK_STR(arg, "}");
    /* recovery: two independent errors both reported */
    err_id(&ID, "cetak(1\ncetak(2\ncetak(3)", 0, &n, NULL, 0);
    CHECK(n == 2);
    /* lexer errors flow through, parser doesn't double-report */
    err_id(&ID, "x = @", 0, &n, arg, sizeof arg);
    CHECK(n == 2); /* E001 from lexer + E004 (expression missing) */
    /* pathological input terminates */
    err_id(&ID, "{ { { jika jika ) ) fungsi", 0, &n, NULL, 0);
    CHECK(n >= 1);

    /* dump includes localized error lines */
    {
        int nerr = 0;
        char *out = aksa_dump_ast("cetak(1", &ID, &nerr);
        CHECK(nerr == 1);
        CHECK(strstr(out, "! E005") != NULL);
        CHECK(strstr(out, "yang hilang") != NULL);
        free(out);
    }

    return TEST_SUMMARY();
}
