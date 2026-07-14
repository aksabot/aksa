#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TOK_NAMES[TOK_COUNT] = {
    "EOF", "ERROR", "IDENT", "NUMBER", "STRING",
    "IF", "ELSE", "REPEAT", "WHILE", "FUNCTION", "RETURN",
    "TRUE", "FALSE", "AND", "OR", "NOT", "VAR", "BREAK",
    "LPAREN", "RPAREN", "LBRACE", "RBRACE", "COMMA",
    "ASSIGN", "EQ", "NEQ", "LT", "GT", "LE", "GE",
    "PLUS", "MINUS", "STAR", "SLASH", "PERCENT",
};

const char *aksa_tok_name(AksaTokKind k) {
    return (k >= 0 && k < TOK_COUNT) ? TOK_NAMES[k] : "?";
}

static int is_cont(unsigned char c) { return (c & 0xC0) == 0x80; }
/* any non-ASCII byte counts as an identifier letter; switch to Unicode
   category tables only if this ever misfires for real kids' code */
static int is_ident_start(unsigned char c) { return isalpha(c) || c == '_' || c >= 0x80; }
static int is_ident_char(unsigned char c) { return isalnum(c) || c == '_' || c >= 0x80; }

void aksa_lexer_init(AksaLexer *lx, const char *src, const AksaLocale *loc, AksaErrors *errs) {
    lx->src = lx->cur = src;
    lx->line = 1;
    lx->col = 1;
    lx->loc = loc;
    lx->errs = errs;
}

static char peek(AksaLexer *lx) { return *lx->cur; }
static char peek2(AksaLexer *lx) { return lx->cur[0] ? lx->cur[1] : 0; }

static void advance(AksaLexer *lx) {
    if (*lx->cur == '\n') {
        lx->line++;
        lx->col = 1;
    } else if (!is_cont((unsigned char)lx->cur[1])) {
        lx->col++; /* count codepoints: bump col only at the end of a UTF-8 sequence */
    }
    lx->cur++;
}

static void skip_ws(AksaLexer *lx) {
    for (;;) {
        char c = peek(lx);
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') advance(lx);
        else if (c == '/' && peek2(lx) == '/') {
            while (peek(lx) && peek(lx) != '\n') advance(lx);
        } else return;
    }
}

static AksaToken tok_at(AksaLexer *lx, AksaTokKind k, const char *start, int line, int col) {
    AksaToken t;
    t.kind = k;
    t.line = line;
    t.col = col;
    t.start = start;
    t.len = (int)(lx->cur - start);
    t.num = 0;
    return t;
}

static AksaToken lex_error(AksaLexer *lx, const char *id, const char *start, int line, int col) {
    char arg[64];
    int n = (int)(lx->cur - start);
    if (n > (int)sizeof arg - 1) n = (int)sizeof arg - 1;
    memcpy(arg, start, (size_t)n);
    arg[n] = 0;
    aksa_errors_add(lx->errs, id, line, col, arg);
    return tok_at(lx, TOK_ERROR, start, line, col);
}

AksaToken aksa_lex_next(AksaLexer *lx) {
    skip_ws(lx);
    const char *start = lx->cur;
    int line = lx->line, col = lx->col;
    char c = peek(lx);

    if (!c) return tok_at(lx, TOK_EOF, start, line, col);

    if (is_ident_start((unsigned char)c)) {
        while (is_ident_char((unsigned char)peek(lx))) advance(lx);
        int kw = aksa_locale_keyword(lx->loc, start, (int)(lx->cur - start));
        return tok_at(lx, kw >= 0 ? (AksaTokKind)kw : TOK_IDENT, start, line, col);
    }

    if (isdigit((unsigned char)c)) {
        while (isdigit((unsigned char)peek(lx))) advance(lx);
        if (peek(lx) == '.' && isdigit((unsigned char)peek2(lx))) {
            advance(lx);
            while (isdigit((unsigned char)peek(lx))) advance(lx);
        }
        if (is_ident_start((unsigned char)peek(lx))) { /* e.g. "12abc" */
            while (is_ident_char((unsigned char)peek(lx))) advance(lx);
            return lex_error(lx, "E003", start, line, col);
        }
        AksaToken t = tok_at(lx, TOK_NUMBER, start, line, col);
        t.num = strtod(start, NULL);
        return t;
    }

    if (c == '"') {
        advance(lx);
        const char *content = lx->cur;
        while (peek(lx) && peek(lx) != '"' && peek(lx) != '\n') {
            if (peek(lx) == '\\' && peek2(lx)) advance(lx); /* keep \" from closing */
            advance(lx);
        }
        if (peek(lx) != '"') return lex_error(lx, "E002", start, line, col);
        AksaToken t = tok_at(lx, TOK_STRING, content, line, col);
        advance(lx); /* closing quote */
        return t;
    }

    advance(lx);
    switch (c) {
    case '(': return tok_at(lx, TOK_LPAREN, start, line, col);
    case ')': return tok_at(lx, TOK_RPAREN, start, line, col);
    case '{': return tok_at(lx, TOK_LBRACE, start, line, col);
    case '}': return tok_at(lx, TOK_RBRACE, start, line, col);
    case ',': return tok_at(lx, TOK_COMMA, start, line, col);
    case '+': return tok_at(lx, TOK_PLUS, start, line, col);
    case '-': return tok_at(lx, TOK_MINUS, start, line, col);
    case '*': return tok_at(lx, TOK_STAR, start, line, col);
    case '/': return tok_at(lx, TOK_SLASH, start, line, col);
    case '%': return tok_at(lx, TOK_PERCENT, start, line, col);
    case '=':
        if (peek(lx) == '=') { advance(lx); return tok_at(lx, TOK_EQ, start, line, col); }
        return tok_at(lx, TOK_ASSIGN, start, line, col);
    case '<':
        if (peek(lx) == '=') { advance(lx); return tok_at(lx, TOK_LE, start, line, col); }
        return tok_at(lx, TOK_LT, start, line, col);
    case '>':
        if (peek(lx) == '=') { advance(lx); return tok_at(lx, TOK_GE, start, line, col); }
        return tok_at(lx, TOK_GT, start, line, col);
    case '!':
        if (peek(lx) == '=') { advance(lx); return tok_at(lx, TOK_NEQ, start, line, col); }
        return lex_error(lx, "E001", start, line, col);
    }

    /* unknown character (always ASCII: non-ASCII bytes lex as identifiers) */
    return lex_error(lx, "E001", start, line, col);
}

char *aksa_dump_tokens(const char *src, const AksaLocale *loc, int *nerrors) {
    AksaErrors errs = {0};
    AksaLexer lx;
    aksa_lexer_init(&lx, src, loc, &errs);

    size_t cap = 1024, len = 0;
    char *buf = malloc(cap);
    buf[0] = 0;
    char lineb[320];

    for (;;) {
        AksaToken t = aksa_lex_next(&lx);
        if (t.kind == TOK_IDENT || t.kind == TOK_NUMBER || t.kind == TOK_STRING || t.kind == TOK_ERROR)
            snprintf(lineb, sizeof lineb, "%d:%d %s '%.*s'\n", t.line, t.col,
                     aksa_tok_name(t.kind), t.len > 200 ? 200 : t.len, t.start);
        else
            snprintf(lineb, sizeof lineb, "%d:%d %s\n", t.line, t.col, aksa_tok_name(t.kind));
        aksa_sb_put(&buf, &len, &cap, lineb);
        if (t.kind == TOK_EOF) break;
    }

    aksa_errors_dump(loc, &errs, &buf, &len, &cap);

    if (nerrors) *nerrors = errs.count;
    return buf;
}
