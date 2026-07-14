#include "checker.h"
#include "lexer.h"
#include <string.h>

/* builtin canonical name → argument count (-1 = any) */
static const struct { const char *canon; int min, max; } BUILTIN_ARITY[] = {
    {"print", 0, -1}, {"ask", 0, 1},
    {"forward", 1, 1}, {"backward", 1, 1}, {"turn_right", 1, 1},
    {"turn_left", 1, 1}, {"color", 1, 1}, {"pen_up", 0, 0}, {"pen_down", 0, 0},
    {"pin_on", 1, 1}, {"pin_off", 1, 1}, {"pin_read", 1, 1},
    {"pin_read_analog", 1, 1}, {"wait", 1, 1},
};

/* value types for the lint: 'b'ool, 'n'umber, 's'tring, '?' unknown.
   Unknown never triggers an error — no false alarms on variables/calls. */

typedef struct {
    const AksaLocale *loc;
    AksaErrors *errs;
    const AksaNode *funcs[128];
    int nfuncs;
    const char *globals[256]; /* every name assigned at top level */
    int globaldef[256];       /* 1 once the assignment has been walked */
    int nglobals;
    const char *locals[64];   /* current function: params + assigned names */
    int localdef[64];
    int nlocals;
    int infunc, inloop;
} Ck;

static void err(Ck *c, const AksaNode *n, const char *id, const char *arg) {
    aksa_errors_add(c->errs, id, n->line, n->col, arg);
}

static const AksaNode *find_func(Ck *c, const char *name) {
    for (int i = 0; i < c->nfuncs; i++)
        if (strcmp(c->funcs[i]->str, name) == 0) return c->funcs[i];
    return NULL;
}

static int find_name(const char **names, int count, const char *name) {
    for (int i = 0; i < count; i++)
        if (strcmp(names[i], name) == 0) return i;
    return -1;
}

/* pre-pass: collect all function definitions (any depth, they all become
   globals at runtime) and every name assigned at top level */
static void collect(Ck *c, const AksaNode *n, int infunc) {
    if (!n) return;
    if (n->kind == AST_FUNC) {
        if (find_func(c, n->str) ||
            aksa_locale_builtin(c->loc, n->str, (int)strlen(n->str)))
            err(c, n, "E111", n->str);
        else if (c->nfuncs < 128)
            c->funcs[c->nfuncs++] = n;
        for (int i = 0; i < n->nlist; i++) /* duplicate parameter names */
            for (int j = i + 1; j < n->nlist; j++)
                if (strcmp(n->list[i]->str, n->list[j]->str) == 0)
                    err(c, n->list[j], "E111", n->list[j]->str);
        collect(c, n->a, 1);
        return;
    }
    if (n->kind == AST_ASSIGN && !infunc &&
        find_name(c->globals, c->nglobals, n->str) < 0 && c->nglobals < 256)
        c->globals[c->nglobals++] = n->str;
    collect(c, n->a, infunc);
    collect(c, n->b, infunc);
    collect(c, n->c, infunc);
    for (int i = 0; i < n->nlist; i++) collect(c, n->list[i], infunc);
}

static void check_name(Ck *c, const AksaNode *n) {
    if (c->infunc) {
        int slot = find_name(c->locals, c->nlocals, n->str);
        if (slot >= 0) {
            if (!c->localdef[slot]) err(c, n, "E100", n->str);
            return;
        }
        /* globals may be assigned before this function is ever called */
        if (find_name(c->globals, c->nglobals, n->str) >= 0) return;
    } else {
        int g = find_name(c->globals, c->nglobals, n->str);
        if (g >= 0 && c->globaldef[g]) return;
    }
    if (find_func(c, n->str)) return;
    err(c, n, "E100", n->str);
}

static char check_expr(Ck *c, const AksaNode *n);

static void check_call(Ck *c, const AksaNode *n) {
    for (int i = 0; i < n->nlist; i++) check_expr(c, n->list[i]);
    const AksaNode *fn = find_func(c, n->str);
    if (fn) {
        if (n->nlist != fn->nlist) err(c, n, "E103", n->str);
        return;
    }
    const char *canon = aksa_locale_builtin(c->loc, n->str, (int)strlen(n->str));
    if (!canon) {
        err(c, n, "E102", n->str);
        return;
    }
    for (size_t i = 0; i < sizeof BUILTIN_ARITY / sizeof BUILTIN_ARITY[0]; i++)
        if (strcmp(BUILTIN_ARITY[i].canon, canon) == 0) {
            if (n->nlist < BUILTIN_ARITY[i].min ||
                (BUILTIN_ARITY[i].max >= 0 && n->nlist > BUILTIN_ARITY[i].max))
                err(c, n, "E103", n->str);
            return;
        }
}

static char check_expr(Ck *c, const AksaNode *n) {
    switch (n->kind) {
    case AST_NUM: return 'n';
    case AST_STR: return 's';
    case AST_BOOL: return 'b';
    case AST_NAME:
        check_name(c, n);
        return '?';
    case AST_CALL:
        check_call(c, n);
        return '?';
    case AST_UNOP: {
        char t = check_expr(c, n->a);
        if (n->op == TOK_NOT) {
            if (t != 'b' && t != '?') err(c, n, "E104", "");
            return 'b';
        }
        if (t != 'n' && t != '?') err(c, n, "E104", "");
        return 'n';
    }
    case AST_BINOP: {
        char ta = check_expr(c, n->a), tb = check_expr(c, n->b);
        switch (n->op) {
        case TOK_AND: case TOK_OR:
            if ((ta != 'b' && ta != '?') || (tb != 'b' && tb != '?'))
                err(c, n, "E104", "");
            return 'b';
        case TOK_EQ: case TOK_NEQ:
            return 'b';
        case TOK_LT: case TOK_GT: case TOK_LE: case TOK_GE:
            if ((ta != 'n' && ta != '?') || (tb != 'n' && tb != '?'))
                err(c, n, "E104", "");
            return 'b';
        case TOK_PLUS:
            if (ta == 's' || tb == 's') return 's'; /* concat coerces the other side */
            if ((ta != 'n' && ta != '?') || (tb != 'n' && tb != '?'))
                err(c, n, "E104", "");
            return ta == '?' && tb == '?' ? '?' : 'n';
        default: /* - * / % */
            if ((ta != 'n' && ta != '?') || (tb != 'n' && tb != '?'))
                err(c, n, "E104", "");
            return 'n';
        }
    }
    default:
        return '?';
    }
}

static void mark_assigned(Ck *c, const char *name) {
    if (c->infunc) {
        int slot = find_name(c->locals, c->nlocals, name);
        if (slot >= 0) c->localdef[slot] = 1;
    } else {
        int g = find_name(c->globals, c->nglobals, name);
        if (g >= 0) c->globaldef[g] = 1;
    }
}

/* function-scoped variables: every name assigned in the body (as in the VM) */
static void collect_locals(Ck *c, const AksaNode *n) {
    if (!n || n->kind == AST_FUNC) return;
    if (n->kind == AST_ASSIGN && find_name(c->locals, c->nlocals, n->str) < 0 &&
        c->nlocals < 64) {
        c->locals[c->nlocals] = n->str;
        c->localdef[c->nlocals++] = 0;
    }
    collect_locals(c, n->a);
    collect_locals(c, n->b);
    collect_locals(c, n->c);
    for (int i = 0; i < n->nlist; i++) collect_locals(c, n->list[i]);
}

static void check_stmt(Ck *c, const AksaNode *n) {
    switch (n->kind) {
    case AST_ASSIGN:
        check_expr(c, n->a);
        mark_assigned(c, n->str);
        break;
    case AST_IF: {
        char t = check_expr(c, n->a);
        if (t != 'b' && t != '?') err(c, n, "E105", "");
        check_stmt(c, n->b);
        if (n->c) check_stmt(c, n->c);
        break;
    }
    case AST_WHILE: {
        char t = check_expr(c, n->a);
        if (t != 'b' && t != '?') err(c, n, "E105", "");
        c->inloop++;
        check_stmt(c, n->b);
        c->inloop--;
        break;
    }
    case AST_REPEAT: {
        char t = check_expr(c, n->a);
        if (t != 'n' && t != '?') err(c, n, "E110", "");
        c->inloop++;
        check_stmt(c, n->b);
        c->inloop--;
        break;
    }
    case AST_FUNC: {
        const char *pnames[64];
        int pdef[64];
        memcpy(pnames, c->locals, sizeof pnames);
        memcpy(pdef, c->localdef, sizeof pdef);
        int pn = c->nlocals, pinfunc = c->infunc, pinloop = c->inloop;

        c->nlocals = 0;
        for (int i = 0; i < n->nlist && i < 64; i++) {
            c->locals[c->nlocals] = n->list[i]->str;
            c->localdef[c->nlocals++] = 1; /* params arrive defined */
        }
        collect_locals(c, n->a);
        c->infunc = 1;
        c->inloop = 0; /* berhenti can't jump out of a function */
        check_stmt(c, n->a);

        memcpy(c->locals, pnames, sizeof pnames);
        memcpy(c->localdef, pdef, sizeof pdef);
        c->nlocals = pn;
        c->infunc = pinfunc;
        c->inloop = pinloop;
        break;
    }
    case AST_RETURN:
        if (!c->infunc) err(c, n, "E108", "");
        if (n->a) check_expr(c, n->a);
        break;
    case AST_BREAK:
        if (!c->inloop) err(c, n, "E107", "");
        break;
    case AST_BLOCK:
    case AST_PROGRAM:
        for (int i = 0; i < n->nlist; i++) check_stmt(c, n->list[i]);
        break;
    default:
        check_expr(c, n);
    }
}

void aksa_check(const AksaNode *prog, const AksaLocale *loc, AksaErrors *errs) {
    Ck c = {0};
    c.loc = loc;
    c.errs = errs;
    collect(&c, prog, 0);
    check_stmt(&c, prog);
}
