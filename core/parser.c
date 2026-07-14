#include "parser.h"
#include "lexer.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    AksaLexer lx;
    AksaToken cur, next;
    AksaErrors *errs;
    int panic; /* suppress cascading errors until the next sync point */
} Par;

static void padvance(Par *p) {
    p->cur = p->next;
    /* TOK_ERROR was already reported by the lexer — skip, don't re-report */
    do p->next = aksa_lex_next(&p->lx);
    while (p->next.kind == TOK_ERROR);
}

static int match(Par *p, AksaTokKind k) {
    if (p->cur.kind != k) return 0;
    padvance(p);
    return 1;
}

static void perr(Par *p, const char *id, const char *arg) {
    if (p->panic) return;
    p->panic = 1;
    aksa_errors_add(p->errs, id, p->cur.line, p->cur.col, arg);
}

static void unexpected(Par *p) {
    char arg[64];
    int n = p->cur.len < 63 ? p->cur.len : 63;
    memcpy(arg, p->cur.start, (size_t)n);
    arg[n] = 0;
    perr(p, "E004", p->cur.kind == TOK_EOF ? "EOF" : arg);
}

static int expect(Par *p, AksaTokKind k, const char *lit) {
    if (match(p, k)) return 1;
    perr(p, "E005", lit);
    return 0;
}

static void sync(Par *p) {
    p->panic = 0;
    while (p->cur.kind != TOK_EOF) {
        switch (p->cur.kind) {
        case TOK_IF: case TOK_REPEAT: case TOK_WHILE: case TOK_FUNCTION:
        case TOK_RETURN: case TOK_BREAK: case TOK_VAR: case TOK_RBRACE:
        case TOK_IDENT:
            return;
        default:
            padvance(p);
        }
    }
}

static char *dup_slice(const char *s, int len) {
    char *out = malloc((size_t)len + 1);
    memcpy(out, s, (size_t)len);
    out[len] = 0;
    return out;
}

/* string escapes: \n \t; any other \x becomes x (so \" and \\ just work) */
static char *unescape(const char *s, int len) {
    char *out = malloc((size_t)len + 1);
    int o = 0;
    for (int i = 0; i < len; i++) {
        char c = s[i];
        if (c == '\\' && i + 1 < len) {
            i++;
            c = s[i] == 'n' ? '\n' : s[i] == 't' ? '\t' : s[i];
        }
        out[o++] = c;
    }
    out[o] = 0;
    return out;
}

static AksaNode *expr(Par *p);
static AksaNode *block(Par *p);

static AksaNode *primary(Par *p) {
    AksaToken t = p->cur;
    switch (t.kind) {
    case TOK_NUMBER: {
        padvance(p);
        AksaNode *n = aksa_node_new(AST_NUM, t.line, t.col);
        n->num = t.num;
        return n;
    }
    case TOK_STRING: {
        padvance(p);
        AksaNode *n = aksa_node_new(AST_STR, t.line, t.col);
        n->str = unescape(t.start, t.len);
        return n;
    }
    case TOK_TRUE:
    case TOK_FALSE: {
        padvance(p);
        AksaNode *n = aksa_node_new(AST_BOOL, t.line, t.col);
        n->num = t.kind == TOK_TRUE;
        return n;
    }
    case TOK_IDENT: {
        padvance(p);
        if (match(p, TOK_LPAREN)) {
            AksaNode *n = aksa_node_new(AST_CALL, t.line, t.col);
            n->str = dup_slice(t.start, t.len);
            if (p->cur.kind != TOK_RPAREN) {
                do aksa_node_push(n, expr(p));
                while (match(p, TOK_COMMA));
            }
            expect(p, TOK_RPAREN, ")");
            return n;
        }
        AksaNode *n = aksa_node_new(AST_NAME, t.line, t.col);
        n->str = dup_slice(t.start, t.len);
        return n;
    }
    case TOK_LPAREN: {
        padvance(p);
        AksaNode *n = expr(p);
        expect(p, TOK_RPAREN, ")");
        return n;
    }
    default:
        unexpected(p);
        /* dummy node so callers never see NULL; errs already records it */
        return aksa_node_new(AST_NUM, t.line, t.col);
    }
}

static AksaNode *unary_expr(Par *p) {
    if (p->cur.kind == TOK_MINUS) {
        AksaToken t = p->cur;
        padvance(p);
        AksaNode *n = aksa_node_new(AST_UNOP, t.line, t.col);
        n->op = TOK_MINUS;
        n->a = unary_expr(p);
        return n;
    }
    return primary(p);
}

static AksaNode *binop(Par *p, AksaNode *lhs, int op, AksaNode *(*sub)(Par *)) {
    AksaNode *n = aksa_node_new(AST_BINOP, p->cur.line, p->cur.col);
    padvance(p); /* the operator */
    n->op = op;
    n->a = lhs;
    n->b = sub(p);
    return n;
}

static AksaNode *mul_expr(Par *p) {
    AksaNode *n = unary_expr(p);
    while (p->cur.kind == TOK_STAR || p->cur.kind == TOK_SLASH || p->cur.kind == TOK_PERCENT)
        n = binop(p, n, p->cur.kind, unary_expr);
    return n;
}

static AksaNode *add_expr(Par *p) {
    AksaNode *n = mul_expr(p);
    while (p->cur.kind == TOK_PLUS || p->cur.kind == TOK_MINUS)
        n = binop(p, n, p->cur.kind, mul_expr);
    return n;
}

static AksaNode *cmp_expr(Par *p) {
    AksaNode *n = add_expr(p);
    while (p->cur.kind == TOK_EQ || p->cur.kind == TOK_NEQ || p->cur.kind == TOK_LT ||
           p->cur.kind == TOK_GT || p->cur.kind == TOK_LE || p->cur.kind == TOK_GE)
        n = binop(p, n, p->cur.kind, add_expr);
    return n;
}

static AksaNode *not_expr(Par *p) {
    if (p->cur.kind == TOK_NOT) {
        AksaToken t = p->cur;
        padvance(p);
        AksaNode *n = aksa_node_new(AST_UNOP, t.line, t.col);
        n->op = TOK_NOT;
        n->a = not_expr(p);
        return n;
    }
    return cmp_expr(p);
}

static AksaNode *and_expr(Par *p) {
    AksaNode *n = not_expr(p);
    while (p->cur.kind == TOK_AND) n = binop(p, n, TOK_AND, not_expr);
    return n;
}

static AksaNode *expr(Par *p) {
    AksaNode *n = and_expr(p);
    while (p->cur.kind == TOK_OR) n = binop(p, n, TOK_OR, and_expr);
    return n;
}

static int starts_expr(AksaTokKind k) {
    return k == TOK_NUMBER || k == TOK_STRING || k == TOK_TRUE || k == TOK_FALSE ||
           k == TOK_IDENT || k == TOK_LPAREN || k == TOK_MINUS || k == TOK_NOT;
}

static AksaNode *statement(Par *p);

static AksaNode *block(Par *p) {
    AksaNode *b = aksa_node_new(AST_BLOCK, p->cur.line, p->cur.col);
    if (!expect(p, TOK_LBRACE, "{")) return b;
    while (p->cur.kind != TOK_RBRACE && p->cur.kind != TOK_EOF) {
        const char *before = p->cur.start;
        aksa_node_push(b, statement(p));
        if (p->panic) sync(p);
        if (p->cur.start == before && p->cur.kind != TOK_RBRACE) padvance(p);
    }
    expect(p, TOK_RBRACE, "}");
    return b;
}

static AksaNode *assign_stmt(Par *p, int decl) {
    AksaToken name = p->cur;
    AksaNode *n = aksa_node_new(AST_ASSIGN, name.line, name.col);
    n->num = decl;
    if (p->cur.kind != TOK_IDENT) {
        unexpected(p);
        n->str = dup_slice("?", 1);
        return n;
    }
    n->str = dup_slice(name.start, name.len);
    padvance(p);
    expect(p, TOK_ASSIGN, "=");
    n->a = expr(p);
    return n;
}

static AksaNode *if_stmt(Par *p) {
    AksaToken t = p->cur;
    padvance(p); /* IF */
    AksaNode *n = aksa_node_new(AST_IF, t.line, t.col);
    expect(p, TOK_LPAREN, "(");
    n->a = expr(p);
    expect(p, TOK_RPAREN, ")");
    n->b = block(p);
    if (match(p, TOK_ELSE))
        n->c = p->cur.kind == TOK_IF ? if_stmt(p) : block(p);
    return n;
}

static AksaNode *statement(Par *p) {
    AksaToken t = p->cur;
    switch (t.kind) {
    case TOK_IF:
        return if_stmt(p);
    case TOK_REPEAT: {
        padvance(p);
        AksaNode *n = aksa_node_new(AST_REPEAT, t.line, t.col);
        n->a = expr(p);
        n->b = block(p);
        return n;
    }
    case TOK_WHILE: {
        padvance(p);
        AksaNode *n = aksa_node_new(AST_WHILE, t.line, t.col);
        expect(p, TOK_LPAREN, "(");
        n->a = expr(p);
        expect(p, TOK_RPAREN, ")");
        n->b = block(p);
        return n;
    }
    case TOK_FUNCTION: {
        padvance(p);
        AksaNode *n = aksa_node_new(AST_FUNC, t.line, t.col);
        if (p->cur.kind == TOK_IDENT) {
            n->str = dup_slice(p->cur.start, p->cur.len);
            padvance(p);
        } else {
            unexpected(p);
            n->str = dup_slice("?", 1);
        }
        expect(p, TOK_LPAREN, "(");
        if (p->cur.kind != TOK_RPAREN) {
            do {
                if (p->cur.kind != TOK_IDENT) {
                    unexpected(p);
                    break;
                }
                AksaNode *param = aksa_node_new(AST_NAME, p->cur.line, p->cur.col);
                param->str = dup_slice(p->cur.start, p->cur.len);
                aksa_node_push(n, param);
                padvance(p);
            } while (match(p, TOK_COMMA));
        }
        expect(p, TOK_RPAREN, ")");
        n->a = block(p);
        return n;
    }
    case TOK_RETURN: {
        padvance(p);
        AksaNode *n = aksa_node_new(AST_RETURN, t.line, t.col);
        if (starts_expr(p->cur.kind)) n->a = expr(p);
        return n;
    }
    case TOK_BREAK:
        padvance(p);
        return aksa_node_new(AST_BREAK, t.line, t.col);
    case TOK_VAR:
        padvance(p);
        return assign_stmt(p, 1);
    case TOK_IDENT:
        if (p->next.kind == TOK_ASSIGN) return assign_stmt(p, 0);
        return expr(p); /* expression statement (usually a call) */
    default:
        if (starts_expr(t.kind)) return expr(p);
        unexpected(p);
        return aksa_node_new(AST_NUM, t.line, t.col); /* dummy, error recorded */
    }
}

AksaNode *aksa_parse(const char *src, const AksaLocale *loc, AksaErrors *errs) {
    Par p;
    aksa_lexer_init(&p.lx, src, loc, errs);
    p.errs = errs;
    p.panic = 0;
    p.next.kind = TOK_EOF;
    padvance(&p); /* prime next */
    padvance(&p); /* prime cur */

    AksaNode *prog = aksa_node_new(AST_PROGRAM, 1, 1);
    while (p.cur.kind != TOK_EOF) {
        const char *before = p.cur.start;
        aksa_node_push(prog, statement(&p));
        if (p.panic) sync(&p);
        if (p.cur.start == before && p.cur.kind != TOK_EOF) padvance(&p);
    }
    return prog;
}

char *aksa_dump_ast(const char *src, const AksaLocale *loc, int *nerrors) {
    AksaErrors errs = {0};
    AksaNode *prog = aksa_parse(src, loc, &errs);
    char *buf = aksa_ast_to_string(prog);
    size_t len = strlen(buf), cap = len + 1;
    aksa_sb_put(&buf, &len, &cap, "\n");
    aksa_errors_dump(loc, &errs, &buf, &len, &cap);
    aksa_ast_free(prog);
    if (nerrors) *nerrors = errs.count;
    return buf;
}
