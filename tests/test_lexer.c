#include "test.h"
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

/* lex `src` into kinds[]; returns count (excluding EOF). errs may be NULL. */
static int lex_all(const AksaLocale *loc, const char *src, AksaToken *toks, int max,
                   AksaErrors *errs) {
    AksaErrors local = {0};
    AksaLexer lx;
    aksa_lexer_init(&lx, src, loc, errs ? errs : &local);
    int n = 0;
    for (;;) {
        AksaToken t = aksa_lex_next(&lx);
        if (t.kind == TOK_EOF) return n;
        if (n < max) toks[n++] = t;
    }
}

int main(void) {
    load(&ID, "locales/id.json");
    load(&EN, "locales/en.json");
    AksaToken t[64];
    AksaErrors errs;

    /* all 13 keywords, both locales */
    {
        static const AksaTokKind want[13] = {
            TOK_IF, TOK_ELSE, TOK_REPEAT, TOK_WHILE, TOK_FUNCTION, TOK_RETURN,
            TOK_TRUE, TOK_FALSE, TOK_AND, TOK_OR, TOK_NOT, TOK_VAR, TOK_BREAK};
        int n = lex_all(&ID,
            "jika lainnya ulangi selama fungsi kembali benar salah dan atau bukan buat berhenti",
            t, 64, NULL);
        CHECK(n == 13);
        for (int i = 0; i < 13; i++) CHECK(t[i].kind == want[i]);
        n = lex_all(&EN,
            "if else repeat while function return true false and or not make stop",
            t, 64, NULL);
        CHECK(n == 13);
        for (int i = 0; i < 13; i++) CHECK(t[i].kind == want[i]);
    }

    /* locale invariance: equivalent programs → identical kind streams */
    {
        AksaToken a[64], b[64];
        int na = lex_all(&ID, "ulangi 10 {\n  maju(50)\n  belok_kanan(36)\n}\n", a, 64, NULL);
        int nb = lex_all(&EN, "repeat 10 {\n  forward(50)\n  turn_right(36)\n}\n", b, 64, NULL);
        CHECK(na == nb);
        for (int i = 0; i < na; i++) CHECK(a[i].kind == b[i].kind);
    }

    /* keywords from the other locale are plain identifiers */
    CHECK(lex_all(&ID, "if", t, 64, NULL) == 1);
    CHECK(t[0].kind == TOK_IDENT);

    /* numbers */
    {
        int n = lex_all(&ID, "42 3.14 0", t, 64, NULL);
        CHECK(n == 3);
        CHECK(t[0].kind == TOK_NUMBER && t[0].num == 42.0);
        CHECK(t[1].kind == TOK_NUMBER && t[1].num == 3.14);
        CHECK(t[2].kind == TOK_NUMBER && t[2].num == 0.0);
    }

    /* strings: slice excludes quotes; escaped quote stays inside */
    {
        int n = lex_all(&ID, "tulis(\"halo dunia\")", t, 64, NULL);
        CHECK(n == 4);
        CHECK(t[0].kind == TOK_IDENT);
        CHECK(t[1].kind == TOK_LPAREN);
        CHECK(t[2].kind == TOK_STRING);
        CHECK(t[2].len == 10 && memcmp(t[2].start, "halo dunia", 10) == 0);
        CHECK(t[3].kind == TOK_RPAREN);
        n = lex_all(&ID, "\"a\\\"b\"", t, 64, NULL); /* source: "a\"b" */
        CHECK(n == 1 && t[0].kind == TOK_STRING && t[0].len == 4);
    }

    /* operators */
    {
        static const AksaTokKind want[] = {
            TOK_EQ, TOK_NEQ, TOK_LE, TOK_GE, TOK_LT, TOK_GT, TOK_PLUS, TOK_MINUS,
            TOK_STAR, TOK_SLASH, TOK_PERCENT, TOK_ASSIGN, TOK_LPAREN, TOK_RPAREN,
            TOK_LBRACE, TOK_RBRACE, TOK_COMMA};
        int n = lex_all(&ID, "== != <= >= < > + - * / % = ( ) { } ,", t, 64, NULL);
        CHECK(n == 17);
        for (int i = 0; i < 17; i++) CHECK(t[i].kind == want[i]);
    }

    /* Unicode identifiers, codepoint-based columns */
    {
        int n = lex_all(&ID, "π = 3.14", t, 64, NULL);
        CHECK(n == 3);
        CHECK(t[0].kind == TOK_IDENT && t[0].len == 2); /* π is 2 bytes */
        CHECK(t[0].col == 1);
        CHECK(t[1].kind == TOK_ASSIGN && t[1].col == 3); /* not 4: π = 1 column */
        CHECK(t[2].kind == TOK_NUMBER && t[2].col == 5);
    }

    /* comments and line/col tracking */
    {
        int n = lex_all(&ID, "# komentar\ntulis(1)\n  jika", t, 64, NULL);
        CHECK(n == 5);
        CHECK(t[0].kind == TOK_IDENT && t[0].line == 2 && t[0].col == 1);
        CHECK(t[3].kind == TOK_RPAREN && t[3].line == 2 && t[3].col == 8);
        CHECK(t[4].kind == TOK_IF && t[4].line == 3 && t[4].col == 3);
    }

    /* E001 unknown character (single and multibyte), lexing continues */
    {
        errs.count = 0;
        int n = lex_all(&ID, "1 @ 2", t, 64, &errs);
        CHECK(n == 3);
        CHECK(t[1].kind == TOK_ERROR);
        CHECK(errs.count == 1);
        CHECK_STR(errs.items[0].id, "E001");
        CHECK_STR(errs.items[0].arg, "@");
        CHECK(errs.items[0].line == 1 && errs.items[0].col == 3);
        errs.count = 0;
        n = lex_all(&ID, "€", t, 64, &errs); /* any non-ASCII is an ident letter */
        CHECK(errs.count == 0);
        CHECK(n == 1 && t[0].kind == TOK_IDENT);
        lex_all(&ID, "!x", t, 64, &errs); /* bare ! is not a token */
        CHECK(errs.count == 1);
        CHECK_STR(errs.items[0].id, "E001");
    }

    /* E002 unterminated string (EOF and newline) */
    {
        errs.count = 0;
        lex_all(&ID, "tulis(\"oops", t, 64, &errs);
        CHECK(errs.count == 1);
        CHECK_STR(errs.items[0].id, "E002");
        CHECK(errs.items[0].line == 1 && errs.items[0].col == 7);
        errs.count = 0;
        lex_all(&ID, "\"a\nb", t, 64, &errs);
        CHECK(errs.count == 1);
        CHECK_STR(errs.items[0].id, "E002");
    }

    /* E003 malformed number */
    {
        errs.count = 0;
        int n = lex_all(&ID, "12abc", t, 64, &errs);
        CHECK(n == 1 && t[0].kind == TOK_ERROR);
        CHECK(errs.count == 1);
        CHECK_STR(errs.items[0].id, "E003");
        CHECK_STR(errs.items[0].arg, "12abc");
    }

    /* localized formatting fills placeholders */
    {
        char msg[256];
        errs.count = 0;
        lex_all(&ID, "\n\n@", t, 64, &errs);
        aksa_error_format(&ID, &errs.items[0], msg, sizeof msg);
        CHECK_STR(msg, "Ada tanda yang tidak dikenal: '@' (baris 3)");
        aksa_error_format(&EN, &errs.items[0], msg, sizeof msg);
        CHECK_STR(msg, "There is an unknown symbol: '@' (line 3)");
    }

    /* dump: tokens then errors, error count reported */
    {
        int nerr = -1;
        char *out = aksa_dump_tokens("tulis(\"halo\")", &ID, &nerr);
        CHECK(nerr == 0);
        CHECK(strstr(out, "1:1 IDENT 'tulis'") != NULL);
        CHECK(strstr(out, "1:7 STRING 'halo'") != NULL);
        CHECK(strstr(out, "EOF") != NULL);
        free(out);
        out = aksa_dump_tokens("@", &ID, &nerr);
        CHECK(nerr == 1);
        CHECK(strstr(out, "! E001 1:1 ") != NULL);
        free(out);
    }

    return TEST_SUMMARY();
}
