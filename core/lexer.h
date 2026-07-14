#ifndef AKSA_LEXER_H
#define AKSA_LEXER_H

#include "error.h"
#include "locale.h"

typedef enum {
    TOK_EOF = 0, TOK_ERROR, TOK_IDENT, TOK_NUMBER, TOK_STRING,
    /* keywords — resolved via the locale dictionary, never hardcoded */
    TOK_IF, TOK_ELSE, TOK_REPEAT, TOK_WHILE, TOK_FUNCTION, TOK_RETURN,
    TOK_TRUE, TOK_FALSE, TOK_AND, TOK_OR, TOK_NOT, TOK_VAR, TOK_BREAK,
    /* punctuation & operators */
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE, TOK_COMMA,
    TOK_ASSIGN, TOK_EQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_COUNT
} AksaTokKind;

typedef struct {
    AksaTokKind kind;
    int line, col;     /* 1-based; col counts codepoints, not bytes */
    const char *start; /* slice into the source (strings: content between quotes) */
    int len;
    double num;        /* value for TOK_NUMBER */
} AksaToken;

typedef struct {
    const char *src, *cur;
    int line, col;
    const AksaLocale *loc;
    AksaErrors *errs;
} AksaLexer;

void aksa_lexer_init(AksaLexer *lx, const char *src, const AksaLocale *loc, AksaErrors *errs);
AksaToken aksa_lex_next(AksaLexer *lx);
const char *aksa_tok_name(AksaTokKind k);

/* Tokenize whole source into a malloc'd printable dump (tokens + localized
   errors). Used by both the CLI and the WASM glue. */
char *aksa_dump_tokens(const char *src, const AksaLocale *loc, int *nerrors);

#endif
