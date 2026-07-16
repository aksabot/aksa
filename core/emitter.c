#include "emitter.h"
#include "ast.h"
#include "checker.h"
#include "lexer.h" /* TOK_* operator kinds */
#include "parser.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- name set (dedup helper) ---------- */

typedef struct {
    const char **names;
    int n, cap;
} Set;

static int set_has(const Set *s, const char *name) {
    for (int i = 0; i < s->n; i++)
        if (strcmp(s->names[i], name) == 0) return 1;
    return 0;
}
static void set_add(Set *s, const char *name) {
    if (set_has(s, name)) return;
    if (s->n >= s->cap) {
        s->cap = s->cap ? s->cap * 2 : 8;
        s->names = realloc(s->names, sizeof(char *) * (size_t)s->cap);
    }
    s->names[s->n++] = name;
}

/* ---------- emitter state ---------- */

typedef struct {
    char *buf;
    size_t len, cap;
    const AksaLocale *loc;
    const char *srcname;
    Set locals; /* names that are locals in the current function; empty at top level */
    int repeatctr;
} E;

static void put(E *e, const char *s) { aksa_sb_put(&e->buf, &e->len, &e->cap, s); }
static void putf(E *e, const char *fmt, ...) {
    char tmp[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(tmp, sizeof tmp, fmt, ap);
    va_end(ap);
    put(e, tmp);
}
static void indent(E *e, int lvl) {
    for (int i = 0; i < lvl; i++) put(e, "    ");
}

/* deterministic, injective mangle to a valid C identifier suffix: alnum kept,
   '_' doubled, any other byte → _<hex> (so literal "_" and escaped bytes never
   collide). Unicode kid identifiers (e.g. jumlah_kué) become valid C. */
static void mangle(E *e, const char *name) {
    char b[8];
    for (const unsigned char *p = (const unsigned char *)name; *p; p++) {
        if (isalnum(*p)) {
            b[0] = (char)*p;
            b[1] = 0;
            put(e, b);
        } else if (*p == '_') {
            put(e, "__");
        } else {
            snprintf(b, sizeof b, "_%02x", *p);
            put(e, b);
        }
    }
}

static void emit_var(E *e, const char *name) {
    put(e, set_has(&e->locals, name) ? "l_" : "g_");
    mangle(e, name);
}

static void emit_cstr(E *e, const char *s) {
    put(e, "\"");
    char one[2] = {0, 0};
    for (const char *p = s; *p; p++) {
        switch (*p) {
        case '"': put(e, "\\\""); break;
        case '\\': put(e, "\\\\"); break;
        case '\n': put(e, "\\n"); break;
        case '\t': put(e, "\\t"); break;
        case '\r': put(e, "\\r"); break;
        default:
            one[0] = *p;
            put(e, one); /* raw UTF-8 bytes pass through */
        }
    }
    put(e, "\"");
}

/* ---------- tree walkers ---------- */

/* every function anywhere in the tree (all become flat C functions, matching
   the VM which stores every function as a global) */
static void collect_funcs(const AksaNode *n, Set *out, const AksaNode ***fns,
                          int *nfns, int *capfns) {
    if (!n) return;
    if (n->kind == AST_FUNC) {
        if (*nfns >= *capfns) {
            *capfns = *capfns ? *capfns * 2 : 8;
            *fns = realloc(*fns, sizeof(AksaNode *) * (size_t)*capfns);
        }
        (*fns)[(*nfns)++] = n;
        (void)out;
    }
    collect_funcs(n->a, out, fns, nfns, capfns);
    collect_funcs(n->b, out, fns, nfns, capfns);
    collect_funcs(n->c, out, fns, nfns, capfns);
    for (int i = 0; i < n->nlist; i++)
        collect_funcs(n->list[i], out, fns, nfns, capfns);
}

/* names assigned in this scope, not descending into nested functions
   (mirrors vm.c collect_locals) */
static void collect_assigns(const AksaNode *n, Set *out) {
    if (!n || n->kind == AST_FUNC) return;
    if (n->kind == AST_ASSIGN) set_add(out, n->str);
    collect_assigns(n->a, out);
    collect_assigns(n->b, out);
    collect_assigns(n->c, out);
    for (int i = 0; i < n->nlist; i++) collect_assigns(n->list[i], out);
}

/* ---------- expressions ---------- */

static void emit_expr(E *e, const AksaNode *n);

static const char *binfn(int op) {
    switch (op) {
    case TOK_PLUS: return "ak_add";
    case TOK_MINUS: return "ak_sub";
    case TOK_STAR: return "ak_mul";
    case TOK_SLASH: return "ak_div";
    case TOK_PERCENT: return "ak_mod";
    case TOK_LT: return "ak_lt";
    case TOK_GT: return "ak_gt";
    case TOK_LE: return "ak_le";
    case TOK_GE: return "ak_ge";
    case TOK_EQ: return "ak_eq";
    case TOK_NEQ: return "ak_neq";
    default: return "ak_add";
    }
}

static void emit_call(E *e, const AksaNode *n) {
    const char *canon = aksa_locale_builtin(e->loc, n->str, (int)strlen(n->str));
    if (!canon) { /* user function (checker guarantees it exists) */
        put(e, "fn_");
        mangle(e, n->str);
        put(e, "(");
        for (int i = 0; i < n->nlist; i++) {
            if (i) put(e, ", ");
            emit_expr(e, n->list[i]);
        }
        put(e, ")");
        return;
    }
    if (strcmp(canon, "print") == 0) {
        putf(e, "ak_print(%d", n->nlist);
        for (int i = 0; i < n->nlist; i++) {
            put(e, ", ");
            emit_expr(e, n->list[i]);
        }
        put(e, ")");
        return;
    }
    if (strcmp(canon, "ask") == 0) {
        put(e, "ak_ask(");
        if (n->nlist >= 1) emit_expr(e, n->list[0]);
        else put(e, "ak_nil()");
        put(e, ")");
        return;
    }
    const char *fn = strcmp(canon, "pin_on") == 0            ? "ak_pin_on"
                     : strcmp(canon, "pin_off") == 0         ? "ak_pin_off"
                     : strcmp(canon, "pin_read") == 0        ? "ak_pin_read"
                     : strcmp(canon, "pin_read_analog") == 0 ? "ak_pin_read_analog"
                     : strcmp(canon, "wait") == 0            ? "ak_wait"
                                                             : NULL;
    if (fn) {
        put(e, fn);
        put(e, "(");
        if (n->nlist >= 1) emit_expr(e, n->list[0]);
        else put(e, "ak_nil()");
        put(e, ")");
        return;
    }
    /* turtle builtins: real on device/browser, not here (E106) */
    put(e, "ak_unsupported(");
    emit_cstr(e, n->str);
    putf(e, ", %d)", n->line);
}

static void emit_expr(E *e, const AksaNode *n) {
    switch (n->kind) {
    case AST_NUM: putf(e, "ak_num(%.17g)", n->num); break;
    case AST_STR:
        put(e, "ak_str(");
        emit_cstr(e, n->str);
        put(e, ")");
        break;
    case AST_BOOL: putf(e, "ak_bool(%d)", (int)n->num); break;
    case AST_NAME:
        /* borrow → fresh: a stored string gets its own reference so the
           consuming op can release it without touching the variable's copy */
        put(e, "ak_retain(");
        emit_var(e, n->str);
        put(e, ")");
        break;
    case AST_UNOP:
        put(e, n->op == TOK_NOT ? "ak_not(" : "ak_neg(");
        emit_expr(e, n->a);
        putf(e, ", %d)", n->line);
        break;
    case AST_BINOP:
        if (n->op == TOK_AND || n->op == TOK_OR) {
            put(e, "ak_bool(ak_asbool(");
            emit_expr(e, n->a);
            put(e, n->op == TOK_AND ? ") && ak_asbool(" : ") || ak_asbool(");
            emit_expr(e, n->b);
            put(e, "))");
        } else {
            int hasline = n->op != TOK_EQ && n->op != TOK_NEQ;
            put(e, binfn(n->op));
            put(e, "(");
            emit_expr(e, n->a);
            put(e, ", ");
            emit_expr(e, n->b);
            if (hasline) putf(e, ", %d", n->line);
            put(e, ")");
        }
        break;
    case AST_CALL: emit_call(e, n); break;
    default: put(e, "ak_nil()");
    }
}

/* ---------- statements ---------- */

/* release every local (incl. params) before leaving a function, so a stored
   string's last owner drops it. Globals live to program end (never released). */
static void emit_release_locals(E *e) {
    for (int i = 0; i < e->locals.n; i++) {
        put(e, "ak_release(l_");
        mangle(e, e->locals.names[i]);
        put(e, "); ");
    }
}

static void emit_stmt(E *e, const AksaNode *n, int lvl);

static void emit_block(E *e, const AksaNode *b, int lvl) {
    for (int i = 0; i < b->nlist; i++) emit_stmt(e, b->list[i], lvl);
}

static void emit_stmt(E *e, const AksaNode *n, int lvl) {
    if (n->kind == AST_FUNC) return; /* hoisted to file scope */

    putf(e, "#line %d \"%s\"\n", n->line, e->srcname);
    indent(e, lvl);

    switch (n->kind) {
    case AST_ASSIGN:
        put(e, "ak_assign(&");
        emit_var(e, n->str);
        put(e, ", ");
        emit_expr(e, n->a);
        put(e, ");\n");
        break;
    case AST_IF:
        put(e, "if (ak_truth(");
        emit_expr(e, n->a);
        put(e, ")) {\n");
        emit_block(e, n->b, lvl + 1);
        indent(e, lvl);
        put(e, "}");
        if (n->c) {
            put(e, " else {\n");
            if (n->c->kind == AST_IF) emit_stmt(e, n->c, lvl + 1);
            else emit_block(e, n->c, lvl + 1);
            indent(e, lvl);
            put(e, "}");
        }
        put(e, "\n");
        break;
    case AST_WHILE:
        put(e, "while (ak_truth(");
        emit_expr(e, n->a);
        put(e, ")) {\n");
        emit_block(e, n->b, lvl + 1);
        indent(e, lvl);
        put(e, "}\n");
        break;
    case AST_REPEAT: {
        int r = e->repeatctr++;
        put(e, "{ double _r");
        putf(e, "%d = ak_count(", r);
        emit_expr(e, n->a);
        put(e, ");\n");
        indent(e, lvl + 1);
        put(e, "for (;;) {\n");
        indent(e, lvl + 2);
        putf(e, "if (_r%d < 1) break;\n", r);
        emit_block(e, n->b, lvl + 2);
        indent(e, lvl + 2);
        putf(e, "_r%d -= 1;\n", r);
        indent(e, lvl + 1);
        put(e, "}\n");
        indent(e, lvl);
        put(e, "}\n");
        break;
    }
    case AST_BREAK: put(e, "break;\n"); break;
    case AST_RETURN:
        /* compute the value (it may read locals), then release locals, return */
        put(e, "{ Ak _rv = ");
        if (n->a) emit_expr(e, n->a);
        else put(e, "ak_nil()");
        put(e, "; ");
        emit_release_locals(e);
        put(e, "return _rv; }\n");
        break;
    case AST_BLOCK:
        emit_block(e, n, lvl);
        break;
    default: /* expression statement: drop the discarded (fresh) result */
        put(e, "ak_release(");
        emit_expr(e, n);
        put(e, ");\n");
    }
}

/* ---------- function definitions ---------- */

static int is_param(const AksaNode *fn, const char *name) {
    for (int i = 0; i < fn->nlist; i++)
        if (strcmp(fn->list[i]->str, name) == 0) return 1;
    return 0;
}

static void emit_proto(E *e, const AksaNode *fn) {
    put(e, "static Ak fn_");
    mangle(e, fn->str);
    put(e, "(");
    if (fn->nlist == 0) put(e, "void");
    for (int i = 0; i < fn->nlist; i++) {
        if (i) put(e, ", ");
        put(e, "Ak l_");
        mangle(e, fn->list[i]->str);
    }
    put(e, ")");
}

static void emit_func(E *e, const AksaNode *fn) {
    Set locals = {0};
    for (int i = 0; i < fn->nlist; i++) set_add(&locals, fn->list[i]->str);
    collect_assigns(fn->a, &locals);
    e->locals = locals;

    emit_proto(e, fn);
    put(e, " {\n");
    for (int i = 0; i < locals.n; i++)
        if (!is_param(fn, locals.names[i])) {
            put(e, "    Ak l_");
            mangle(e, locals.names[i]);
            put(e, " = ak_nil();\n");
        }
    emit_block(e, fn->a, 1);
    put(e, "    ");
    emit_release_locals(e);
    put(e, "return ak_nil();\n}\n\n");

    e->locals = (Set){0};
    free(locals.names);
}

/* ---------- entry point ---------- */

char *aksa_emit_c(const char *src, const AksaLocale *loc, const char *locale_name,
                  AksaErrors *errs, const char *srcname) {
    AksaNode *prog = aksa_parse(src, loc, errs);
    if (errs->count == 0) aksa_check(prog, loc, errs);
    if (errs->count > 0) {
        aksa_ast_free(prog);
        return NULL;
    }

    E e = {0};
    e.cap = 1024;
    e.buf = malloc(e.cap);
    e.buf[0] = 0;
    e.loc = loc;
    e.srcname = srcname;

    put(&e, "/* Generated by aksa emit — do not edit. */\n");
    put(&e, "#include \"rt.h\"\n\n");

    /* globals: every top-level assigned name (functions can read them) */
    Set globals = {0};
    collect_assigns(prog, &globals);
    for (int i = 0; i < globals.n; i++) {
        put(&e, "static Ak g_");
        mangle(&e, globals.names[i]);
        put(&e, " = {0};\n");
    }
    if (globals.n) put(&e, "\n");
    free(globals.names);

    /* functions: prototypes then definitions (flat, forward-declared) */
    const AksaNode **fns = NULL;
    int nfns = 0, capfns = 0;
    collect_funcs(prog, NULL, &fns, &nfns, &capfns);
    for (int i = 0; i < nfns; i++) {
        emit_proto(&e, fns[i]);
        put(&e, ";\n");
    }
    if (nfns) put(&e, "\n");
    for (int i = 0; i < nfns; i++) emit_func(&e, fns[i]);
    free(fns);

    put(&e, "int main(void) {\n");
    putf(&e, "    aksa_rt_init(\"%s\");\n", locale_name);
    for (int i = 0; i < prog->nlist; i++) emit_stmt(&e, prog->list[i], 1);
    put(&e, "    return 0;\n}\n");

    aksa_ast_free(prog);
    return e.buf;
}
