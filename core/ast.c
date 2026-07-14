#include "ast.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AksaNode *aksa_node_new(AksaAstKind kind, int line, int col) {
    AksaNode *n = calloc(1, sizeof *n);
    n->kind = kind;
    n->line = line;
    n->col = col;
    return n;
}

void aksa_node_push(AksaNode *parent, AksaNode *child) {
    parent->list = realloc(parent->list, sizeof(AksaNode *) * (size_t)(parent->nlist + 1));
    parent->list[parent->nlist++] = child;
}

void aksa_ast_free(AksaNode *n) {
    if (!n) return;
    aksa_ast_free(n->a);
    aksa_ast_free(n->b);
    aksa_ast_free(n->c);
    for (int i = 0; i < n->nlist; i++) aksa_ast_free(n->list[i]);
    free(n->list);
    free(n->str);
    free(n);
}

static const char *op_sym(int op) {
    switch (op) {
    case TOK_PLUS: return "+";
    case TOK_MINUS: return "-";
    case TOK_STAR: return "*";
    case TOK_SLASH: return "/";
    case TOK_PERCENT: return "%";
    case TOK_EQ: return "==";
    case TOK_NEQ: return "!=";
    case TOK_LT: return "<";
    case TOK_GT: return ">";
    case TOK_LE: return "<=";
    case TOK_GE: return ">=";
    case TOK_AND: return "and";
    case TOK_OR: return "or";
    case TOK_NOT: return "not";
    default: return "?";
    }
}

static void dump(const AksaNode *n, char **buf, size_t *len, size_t *cap) {
    char tmp[64];
    if (!n) {
        aksa_sb_put(buf, len, cap, "?");
        return;
    }
    switch (n->kind) {
    case AST_PROGRAM:
        for (int i = 0; i < n->nlist; i++) {
            if (i) aksa_sb_put(buf, len, cap, "\n");
            dump(n->list[i], buf, len, cap);
        }
        break;
    case AST_BLOCK:
        aksa_sb_put(buf, len, cap, "(block");
        for (int i = 0; i < n->nlist; i++) {
            aksa_sb_put(buf, len, cap, " ");
            dump(n->list[i], buf, len, cap);
        }
        aksa_sb_put(buf, len, cap, ")");
        break;
    case AST_NUM:
        snprintf(tmp, sizeof tmp, "%g", n->num);
        aksa_sb_put(buf, len, cap, tmp);
        break;
    case AST_STR:
        aksa_sb_put(buf, len, cap, "(str \"");
        aksa_sb_put(buf, len, cap, n->str);
        aksa_sb_put(buf, len, cap, "\")");
        break;
    case AST_BOOL:
        aksa_sb_put(buf, len, cap, n->num ? "true" : "false");
        break;
    case AST_NAME:
        aksa_sb_put(buf, len, cap, n->str);
        break;
    case AST_BINOP:
    case AST_UNOP:
        aksa_sb_put(buf, len, cap, "(");
        aksa_sb_put(buf, len, cap,
                    n->kind == AST_UNOP && n->op == TOK_MINUS ? "neg" : op_sym(n->op));
        aksa_sb_put(buf, len, cap, " ");
        dump(n->a, buf, len, cap);
        if (n->kind == AST_BINOP) {
            aksa_sb_put(buf, len, cap, " ");
            dump(n->b, buf, len, cap);
        }
        aksa_sb_put(buf, len, cap, ")");
        break;
    case AST_CALL:
        aksa_sb_put(buf, len, cap, "(call ");
        aksa_sb_put(buf, len, cap, n->str);
        for (int i = 0; i < n->nlist; i++) {
            aksa_sb_put(buf, len, cap, " ");
            dump(n->list[i], buf, len, cap);
        }
        aksa_sb_put(buf, len, cap, ")");
        break;
    case AST_IF:
        aksa_sb_put(buf, len, cap, "(if ");
        dump(n->a, buf, len, cap);
        aksa_sb_put(buf, len, cap, " ");
        dump(n->b, buf, len, cap);
        if (n->c) {
            aksa_sb_put(buf, len, cap, " ");
            dump(n->c, buf, len, cap);
        }
        aksa_sb_put(buf, len, cap, ")");
        break;
    case AST_REPEAT:
    case AST_WHILE:
        aksa_sb_put(buf, len, cap, n->kind == AST_REPEAT ? "(repeat " : "(while ");
        dump(n->a, buf, len, cap);
        aksa_sb_put(buf, len, cap, " ");
        dump(n->b, buf, len, cap);
        aksa_sb_put(buf, len, cap, ")");
        break;
    case AST_FUNC:
        aksa_sb_put(buf, len, cap, "(func ");
        aksa_sb_put(buf, len, cap, n->str);
        aksa_sb_put(buf, len, cap, " (params");
        for (int i = 0; i < n->nlist; i++) {
            aksa_sb_put(buf, len, cap, " ");
            dump(n->list[i], buf, len, cap);
        }
        aksa_sb_put(buf, len, cap, ") ");
        dump(n->a, buf, len, cap);
        aksa_sb_put(buf, len, cap, ")");
        break;
    case AST_RETURN:
        aksa_sb_put(buf, len, cap, "(return");
        if (n->a) {
            aksa_sb_put(buf, len, cap, " ");
            dump(n->a, buf, len, cap);
        }
        aksa_sb_put(buf, len, cap, ")");
        break;
    case AST_BREAK:
        aksa_sb_put(buf, len, cap, "(break)");
        break;
    case AST_ASSIGN:
        aksa_sb_put(buf, len, cap, n->num ? "(decl " : "(assign ");
        aksa_sb_put(buf, len, cap, n->str);
        aksa_sb_put(buf, len, cap, " ");
        dump(n->a, buf, len, cap);
        aksa_sb_put(buf, len, cap, ")");
        break;
    }
}

char *aksa_ast_to_string(const AksaNode *n) {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    buf[0] = 0;
    dump(n, &buf, &len, &cap);
    return buf;
}
