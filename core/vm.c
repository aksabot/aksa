#include "vm.h"
#include "ast.h"
#include "checker.h"
#include "lexer.h"
#include "parser.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- values ---------- */

typedef enum { V_UNDEF, V_NIL, V_NUM, V_BOOL, V_STR, V_FUNC } VType;

typedef struct Func Func;
typedef struct {
    VType t;
    double num;
    const char *str;
    Func *fn;
} Val;

#define NUMV(x) ((Val){V_NUM, (x), NULL, NULL})
#define BOOLV(x) ((Val){V_BOOL, (x) != 0, NULL, NULL})
#define NILV ((Val){V_NIL, 0, NULL, NULL})

/* ---------- bytecode ---------- */

enum {
    OP_CONST, OP_NIL, OP_TRUE, OP_FALSE, OP_POP,
    OP_GETL, OP_SETL, OP_GETG, OP_SETG,
    OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LE, OP_GE,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_NOT, OP_NEG,
    OP_JMP, OP_JIF, OP_LOOP, OP_REPEAT,
    OP_CALL, OP_RET,
};

typedef struct {
    uint8_t *code;
    int *lines; /* per code byte, for runtime error positions */
    int len, cap;
    Val consts[256]; /* u8 operand — plenty for kid programs */
    int nconsts;
} Chunk;

struct Func {
    Chunk ch;
    int arity, nlocals;
    char *name;
    char *lnames[64]; /* local slot → name, for undefined-variable messages */
};

static void chunk_free(Chunk *ch) {
    for (int i = 0; i < ch->nconsts; i++)
        if (ch->consts[i].t == V_STR) free((char *)ch->consts[i].str);
    free(ch->code);
    free(ch->lines);
}

/* ---------- compiler ---------- */

typedef struct Loop {
    struct Loop *prev;
    int is_repeat;   /* repeat keeps its counter on the stack; break must pop it */
    int breaks[32];  /* jump sites to patch to loop end */
    int nbreaks;
} Loop;

typedef struct {
    const AksaLocale *loc;
    AksaErrors *errs;
    Func *fn;
    const char *localnames[64];
    int nlocals, infunc;
    Loop *loop;
    Func **funcs; /* registry so everything is freed at the end */
    int nfuncs, capfuncs;
    int failed;
} C;

static void cerr(C *c, const AksaNode *n, const char *id, const char *arg) {
    aksa_errors_add(c->errs, id, n->line, n->col, arg);
    c->failed = 1;
}

static void emit(C *c, uint8_t byte, int line) {
    Chunk *ch = &c->fn->ch;
    if (ch->len + 1 > ch->cap) {
        ch->cap = ch->cap ? ch->cap * 2 : 64;
        ch->code = realloc(ch->code, (size_t)ch->cap);
        ch->lines = realloc(ch->lines, (size_t)ch->cap * sizeof(int));
    }
    ch->code[ch->len] = byte;
    ch->lines[ch->len] = line;
    ch->len++;
}

static int add_const(C *c, const AksaNode *n, Val v) {
    Chunk *ch = &c->fn->ch;
    if (v.t == V_STR) /* dedupe names/strings to save slots */
        for (int i = 0; i < ch->nconsts; i++)
            if (ch->consts[i].t == V_STR && strcmp(ch->consts[i].str, v.str) == 0) {
                free((char *)v.str);
                return i;
            }
    if (ch->nconsts >= 256) {
        cerr(c, n, "E109", "");
        return 0;
    }
    ch->consts[ch->nconsts] = v;
    return ch->nconsts++;
}

static int name_const(C *c, const AksaNode *n, const char *name) {
    char *dup = malloc(strlen(name) + 1);
    strcpy(dup, name);
    return add_const(c, n, (Val){V_STR, 0, dup, NULL});
}

static int emit_jump(C *c, uint8_t op, int line) {
    emit(c, op, line);
    emit(c, 0xff, line);
    emit(c, 0xff, line);
    return c->fn->ch.len - 2;
}

static void patch_jump(C *c, int site) {
    int off = c->fn->ch.len - site - 2;
    c->fn->ch.code[site] = (uint8_t)(off >> 8);
    c->fn->ch.code[site + 1] = (uint8_t)(off & 0xff);
}

static void emit_loop(C *c, int start, int line) {
    emit(c, OP_LOOP, line);
    int off = c->fn->ch.len - start + 2;
    emit(c, (uint8_t)(off >> 8), line);
    emit(c, (uint8_t)(off & 0xff), line);
}

static int resolve_local(C *c, const char *name) {
    for (int i = 0; i < c->nlocals; i++)
        if (strcmp(c->localnames[i], name) == 0) return i;
    return -1;
}

/* function-scoped variables: params + every name assigned in the body
   (not descending into nested functions) */
static void collect_locals(C *c, const AksaNode *n) {
    if (!n || n->kind == AST_FUNC) return;
    if (n->kind == AST_ASSIGN && resolve_local(c, n->str) < 0 && c->nlocals < 64)
        c->localnames[c->nlocals++] = n->str;
    collect_locals(c, n->a);
    collect_locals(c, n->b);
    collect_locals(c, n->c);
    for (int i = 0; i < n->nlist; i++) collect_locals(c, n->list[i]);
}

static void compile_expr(C *c, const AksaNode *n);
static void compile_stmt(C *c, const AksaNode *n);

static void compile_expr(C *c, const AksaNode *n) {
    switch (n->kind) {
    case AST_NUM:
        emit(c, OP_CONST, n->line);
        emit(c, (uint8_t)add_const(c, n, NUMV(n->num)), n->line);
        break;
    case AST_STR:
        emit(c, OP_CONST, n->line);
        emit(c, (uint8_t)name_const(c, n, n->str), n->line);
        break;
    case AST_BOOL:
        emit(c, n->num ? OP_TRUE : OP_FALSE, n->line);
        break;
    case AST_NAME: {
        int slot = c->infunc ? resolve_local(c, n->str) : -1;
        if (slot >= 0) {
            emit(c, OP_GETL, n->line);
            emit(c, (uint8_t)slot, n->line);
        } else {
            emit(c, OP_GETG, n->line);
            emit(c, (uint8_t)name_const(c, n, n->str), n->line);
        }
        break;
    }
    case AST_UNOP:
        compile_expr(c, n->a);
        emit(c, n->op == TOK_NOT ? OP_NOT : OP_NEG, n->line);
        break;
    case AST_BINOP:
        if (n->op == TOK_AND) {
            /* a false → whole thing false (operands are bools, VM enforces) */
            compile_expr(c, n->a);
            int skip = emit_jump(c, OP_JIF, n->line);
            compile_expr(c, n->b);
            int end = emit_jump(c, OP_JMP, n->line);
            patch_jump(c, skip);
            emit(c, OP_FALSE, n->line);
            patch_jump(c, end);
        } else if (n->op == TOK_OR) {
            compile_expr(c, n->a);
            int rhs = emit_jump(c, OP_JIF, n->line);
            emit(c, OP_TRUE, n->line);
            int end = emit_jump(c, OP_JMP, n->line);
            patch_jump(c, rhs);
            compile_expr(c, n->b);
            patch_jump(c, end);
        } else {
            compile_expr(c, n->a);
            compile_expr(c, n->b);
            switch (n->op) {
            case TOK_PLUS: emit(c, OP_ADD, n->line); break;
            case TOK_MINUS: emit(c, OP_SUB, n->line); break;
            case TOK_STAR: emit(c, OP_MUL, n->line); break;
            case TOK_SLASH: emit(c, OP_DIV, n->line); break;
            case TOK_PERCENT: emit(c, OP_MOD, n->line); break;
            case TOK_EQ: emit(c, OP_EQ, n->line); break;
            case TOK_NEQ: emit(c, OP_NEQ, n->line); break;
            case TOK_LT: emit(c, OP_LT, n->line); break;
            case TOK_GT: emit(c, OP_GT, n->line); break;
            case TOK_LE: emit(c, OP_LE, n->line); break;
            case TOK_GE: emit(c, OP_GE, n->line); break;
            }
        }
        break;
    case AST_CALL: {
        for (int i = 0; i < n->nlist; i++) compile_expr(c, n->list[i]);
        emit(c, OP_CALL, n->line);
        emit(c, (uint8_t)name_const(c, n, n->str), n->line);
        emit(c, (uint8_t)n->nlist, n->line);
        break;
    }
    default: /* dummy node from parser recovery */
        emit(c, OP_NIL, n->line);
    }
}

static void compile_block(C *c, const AksaNode *b) {
    for (int i = 0; i < b->nlist; i++) compile_stmt(c, b->list[i]);
}

static void compile_func(C *c, const AksaNode *n) {
    Func *fn = calloc(1, sizeof *fn);
    fn->name = malloc(strlen(n->str) + 1);
    strcpy(fn->name, n->str);
    fn->arity = n->nlist;
    if (c->nfuncs >= c->capfuncs) {
        c->capfuncs = c->capfuncs ? c->capfuncs * 2 : 8;
        c->funcs = realloc(c->funcs, sizeof(Func *) * (size_t)c->capfuncs);
    }
    c->funcs[c->nfuncs++] = fn;

    /* save compiler state, compile body in the new function's scope */
    Func *pfn = c->fn;
    const char *pnames[64];
    memcpy(pnames, c->localnames, sizeof pnames);
    int pnlocals = c->nlocals, pinfunc = c->infunc;
    Loop *ploop = c->loop;

    c->fn = fn;
    c->infunc = 1;
    c->loop = NULL;
    c->nlocals = 0;
    for (int i = 0; i < n->nlist && i < 64; i++)
        c->localnames[c->nlocals++] = n->list[i]->str;
    collect_locals(c, n->a);
    fn->nlocals = c->nlocals;
    for (int i = 0; i < c->nlocals; i++) {
        fn->lnames[i] = malloc(strlen(c->localnames[i]) + 1);
        strcpy(fn->lnames[i], c->localnames[i]);
    }
    compile_block(c, n->a);
    emit(c, OP_NIL, n->line);
    emit(c, OP_RET, n->line);

    c->fn = pfn;
    memcpy(c->localnames, pnames, sizeof pnames);
    c->nlocals = pnlocals;
    c->infunc = pinfunc;
    c->loop = ploop;

    /* defining a function = storing it in a global named after it */
    emit(c, OP_CONST, n->line);
    emit(c, (uint8_t)add_const(c, n, (Val){V_FUNC, 0, NULL, fn}), n->line);
    emit(c, OP_SETG, n->line);
    emit(c, (uint8_t)name_const(c, n, n->str), n->line);
}

static void compile_stmt(C *c, const AksaNode *n) {
    switch (n->kind) {
    case AST_ASSIGN: {
        compile_expr(c, n->a);
        int slot = c->infunc ? resolve_local(c, n->str) : -1;
        if (slot >= 0) {
            emit(c, OP_SETL, n->line);
            emit(c, (uint8_t)slot, n->line);
        } else {
            emit(c, OP_SETG, n->line);
            emit(c, (uint8_t)name_const(c, n, n->str), n->line);
        }
        break;
    }
    case AST_IF: {
        compile_expr(c, n->a);
        int elsej = emit_jump(c, OP_JIF, n->line);
        compile_stmt(c, n->b);
        int endj = emit_jump(c, OP_JMP, n->line);
        patch_jump(c, elsej);
        if (n->c) compile_stmt(c, n->c);
        patch_jump(c, endj);
        break;
    }
    case AST_WHILE: {
        int start = c->fn->ch.len;
        compile_expr(c, n->a);
        int exitj = emit_jump(c, OP_JIF, n->line);
        Loop lp = {c->loop, 0, {0}, 0};
        c->loop = &lp;
        compile_stmt(c, n->b);
        c->loop = lp.prev;
        emit_loop(c, start, n->line);
        patch_jump(c, exitj);
        for (int i = 0; i < lp.nbreaks; i++) patch_jump(c, lp.breaks[i]);
        break;
    }
    case AST_REPEAT: {
        compile_expr(c, n->a); /* counter lives on the stack during the loop */
        int start = c->fn->ch.len;
        int exitj = emit_jump(c, OP_REPEAT, n->line);
        Loop lp = {c->loop, 1, {0}, 0};
        c->loop = &lp;
        compile_stmt(c, n->b);
        c->loop = lp.prev;
        emit_loop(c, start, n->line);
        patch_jump(c, exitj);
        for (int i = 0; i < lp.nbreaks; i++) patch_jump(c, lp.breaks[i]);
        break;
    }
    case AST_FUNC:
        compile_func(c, n);
        break;
    case AST_RETURN:
        if (!c->infunc) {
            cerr(c, n, "E108", "");
            break;
        }
        if (n->a) compile_expr(c, n->a);
        else emit(c, OP_NIL, n->line);
        emit(c, OP_RET, n->line);
        break;
    case AST_BREAK: {
        if (!c->loop) {
            cerr(c, n, "E107", "");
            break;
        }
        if (c->loop->is_repeat) emit(c, OP_POP, n->line); /* drop the counter */
        if (c->loop->nbreaks < 32)
            c->loop->breaks[c->loop->nbreaks++] = emit_jump(c, OP_JMP, n->line);
        break;
    }
    case AST_BLOCK:
        compile_block(c, n);
        break;
    default: /* expression statement: evaluate, discard */
        compile_expr(c, n);
        emit(c, OP_POP, n->line);
    }
}

/* ---------- VM ---------- */

#define STACK_MAX 1024
#define FRAMES_MAX 64
#define GLOBALS_MAX 256

typedef struct {
    Func *fn;
    uint8_t *ip;
    Val *base;
} Frame;

typedef struct {
    Val stack[STACK_MAX];
    Val *sp;
    Frame frames[FRAMES_MAX];
    int nframes;
    struct { const char *name; Val v; } globals[GLOBALS_MAX];
    int nglobals;
    const AksaLocale *loc;
    AksaErrors *errs;
    AksaOutFn out;
    AksaInFn in;
    AksaYieldFn yield;
    AksaHostFn host;
    void *user;
    char **owned; /* every runtime-created string, freed when the VM ends */
    int nowned, capowned;
    int failed;
} VM;

static void stdout_out(const char *text, void *user) {
    (void)user;
    fputs(text, stdout);
}

static void stdin_in(char *buf, int bufsz, void *user) {
    (void)user;
    if (!fgets(buf, bufsz, stdin)) buf[0] = 0;
}

static char *own(VM *vm, char *s) {
    if (vm->nowned >= vm->capowned) {
        vm->capowned = vm->capowned ? vm->capowned * 2 : 16;
        vm->owned = realloc(vm->owned, sizeof(char *) * (size_t)vm->capowned);
    }
    vm->owned[vm->nowned++] = s;
    return s;
}

static void rterr(VM *vm, int line, const char *id, const char *arg) {
    aksa_errors_add(vm->errs, id, line, 0, arg);
    vm->failed = 1;
}

static const char *kw_word(const AksaLocale *loc, int tok, const char *fallback) {
    for (int i = 0; i < loc->nkeywords; i++)
        if (loc->keywords[i].tok == tok) return loc->keywords[i].word;
    return fallback;
}

/* value → display text; tmp must hold 64 bytes */
static const char *val_str(VM *vm, Val v, char *tmp) {
    switch (v.t) {
    case V_NUM:
        snprintf(tmp, 64, "%.15g", v.num);
        return tmp;
    case V_BOOL:
        return kw_word(vm->loc, v.num ? TOK_TRUE : TOK_FALSE, v.num ? "true" : "false");
    case V_STR:
        return v.str;
    case V_FUNC:
        return v.fn->name;
    default:
        return "";
    }
}

static Val concat(VM *vm, Val a, Val b) {
    char ta[64], tb[64];
    const char *sa = val_str(vm, a, ta), *sb = val_str(vm, b, tb);
    char *s = malloc(strlen(sa) + strlen(sb) + 1);
    strcpy(s, sa);
    strcat(s, sb);
    return (Val){V_STR, 0, own(vm, s), NULL};
}

static int val_eq(Val a, Val b) {
    if (a.t != b.t) return 0;
    switch (a.t) {
    case V_NUM: return a.num == b.num;
    case V_BOOL: return (a.num != 0) == (b.num != 0);
    case V_STR: return strcmp(a.str, b.str) == 0;
    case V_FUNC: return a.fn == b.fn;
    default: return 1;
    }
}

static Val *global_find(VM *vm, const char *name) {
    for (int i = 0; i < vm->nglobals; i++)
        if (strcmp(vm->globals[i].name, name) == 0) return &vm->globals[i].v;
    return NULL;
}

/* returns 0 on failure (error already recorded) */
static int builtin_call(VM *vm, int line, const char *name, const char *canon,
                        Val *args, int argc, Val *result) {
    *result = NILV;
    if (strcmp(canon, "print") == 0) {
        char tmp[64];
        size_t cap = 64, len = 0;
        char *buf = malloc(cap);
        buf[0] = 0;
        for (int i = 0; i < argc; i++) {
            if (i) aksa_sb_put(&buf, &len, &cap, " ");
            aksa_sb_put(&buf, &len, &cap, val_str(vm, args[i], tmp));
        }
        aksa_sb_put(&buf, &len, &cap, "\n");
        vm->out(buf, vm->user);
        free(buf);
        return 1;
    }
    if (strcmp(canon, "ask") == 0) {
        char tmp[64];
        if (argc > 1) {
            rterr(vm, line, "E103", name);
            return 0;
        }
        if (argc == 1) {
            vm->out(val_str(vm, args[0], tmp), vm->user);
            vm->out(" ", vm->user);
        }
        char buf[256];
        vm->in(buf, sizeof buf, vm->user);
        buf[strcspn(buf, "\r\n")] = 0;
        char *end;
        double d = strtod(buf, &end); /* numeric answers become numbers */
        if (end != buf && *end == 0) {
            *result = NUMV(d);
            return 1;
        }
        char *s = malloc(strlen(buf) + 1);
        strcpy(s, buf);
        *result = (Val){V_STR, 0, own(vm, s), NULL};
        return 1;
    }
    if (vm->host && argc <= 1) {
        double num = 0;
        const char *str = NULL;
        if (argc == 1) {
            if (args[0].t == V_NUM) num = args[0].num;
            else if (args[0].t == V_STR) str = args[0].str;
            else { rterr(vm, line, "E104", ""); return 0; }
        }
        double res = 0;
        if (vm->host(canon, num, str, &res, vm->user)) {
            if (strcmp(canon, "pin_read") == 0 ||
                strcmp(canon, "pin_read_analog") == 0)
                *result = NUMV(res);
            return 1;
        }
    }
    /* turtle/hardware builtins exist but need the browser/device (Phase 2/3) */
    rterr(vm, line, "E106", name);
    return 0;
}

static int vm_execute(VM *vm, Func *script) {
    Frame *f = &vm->frames[0];
    vm->nframes = 1;
    f->fn = script;
    f->ip = script->ch.code;
    f->base = vm->stack;
    vm->sp = vm->stack;
    for (int i = 0; i < script->nlocals; i++) *vm->sp++ = (Val){V_UNDEF, 0, NULL, NULL};

#define LINE() (f->fn->ch.lines[f->ip - f->fn->ch.code - 1])
#define RTFAIL(id, arg) do { rterr(vm, LINE(), id, arg); return 1; } while (0)
#define PUSH(v) do { \
        if (vm->sp - vm->stack >= STACK_MAX) RTFAIL("E109", ""); \
        *vm->sp++ = (v); \
    } while (0)
#define POP() (*--vm->sp)
#define NUMOP(op) do { \
        Val b_ = POP(), a_ = POP(); \
        if (a_.t != V_NUM || b_.t != V_NUM) RTFAIL("E104", ""); \
        PUSH(op); \
    } while (0)

/* how many instructions run between host yields (~1ms of work) */
#define AKSA_YIELD_EVERY 100000

    int budget = AKSA_YIELD_EVERY;
    for (;;) {
        if (vm->yield && --budget <= 0) {
            budget = AKSA_YIELD_EVERY;
            if (vm->yield(vm->user)) return vm->failed; /* stop: halt, no error */
        }
        uint8_t op = *f->ip++;
        switch (op) {
        case OP_CONST: PUSH(f->fn->ch.consts[*f->ip++]); break;
        case OP_NIL: PUSH(NILV); break;
        case OP_TRUE: PUSH(BOOLV(1)); break;
        case OP_FALSE: PUSH(BOOLV(0)); break;
        case OP_POP: (void)POP(); break;
        case OP_GETL: {
            uint8_t slot = *f->ip++;
            Val v = f->base[slot];
            if (v.t == V_UNDEF) RTFAIL("E100", f->fn->lnames[slot]);
            PUSH(v);
            break;
        }
        case OP_SETL: f->base[*f->ip++] = POP(); break;
        case OP_GETG: {
            const char *name = f->fn->ch.consts[*f->ip++].str;
            Val *g = global_find(vm, name);
            if (!g) RTFAIL("E100", name);
            PUSH(*g);
            break;
        }
        case OP_SETG: {
            const char *name = f->fn->ch.consts[*f->ip++].str;
            Val *g = global_find(vm, name);
            if (!g) {
                if (vm->nglobals >= GLOBALS_MAX) RTFAIL("E109", "");
                vm->globals[vm->nglobals].name = name;
                g = &vm->globals[vm->nglobals++].v;
            }
            *g = POP();
            break;
        }
        case OP_EQ: { Val b = POP(), a = POP(); PUSH(BOOLV(val_eq(a, b))); break; }
        case OP_NEQ: { Val b = POP(), a = POP(); PUSH(BOOLV(!val_eq(a, b))); break; }
        case OP_LT: NUMOP(BOOLV(a_.num < b_.num)); break;
        case OP_GT: NUMOP(BOOLV(a_.num > b_.num)); break;
        case OP_LE: NUMOP(BOOLV(a_.num <= b_.num)); break;
        case OP_GE: NUMOP(BOOLV(a_.num >= b_.num)); break;
        case OP_ADD: {
            Val b = POP(), a = POP();
            if (a.t == V_STR || b.t == V_STR) PUSH(concat(vm, a, b));
            else if (a.t == V_NUM && b.t == V_NUM) PUSH(NUMV(a.num + b.num));
            else RTFAIL("E104", "");
            break;
        }
        case OP_SUB: NUMOP(NUMV(a_.num - b_.num)); break;
        case OP_MUL: NUMOP(NUMV(a_.num * b_.num)); break;
        case OP_DIV: {
            Val b = POP(), a = POP();
            if (a.t != V_NUM || b.t != V_NUM) RTFAIL("E104", "");
            if (b.num == 0) RTFAIL("E101", "");
            PUSH(NUMV(a.num / b.num));
            break;
        }
        case OP_MOD: {
            Val b = POP(), a = POP();
            if (a.t != V_NUM || b.t != V_NUM) RTFAIL("E104", "");
            if (b.num == 0) RTFAIL("E101", "");
            PUSH(NUMV(fmod(a.num, b.num)));
            break;
        }
        case OP_NOT: {
            Val a = POP();
            if (a.t != V_BOOL) RTFAIL("E104", "");
            PUSH(BOOLV(!(a.num != 0)));
            break;
        }
        case OP_NEG: {
            Val a = POP();
            if (a.t != V_NUM) RTFAIL("E104", "");
            PUSH(NUMV(-a.num));
            break;
        }
        case OP_JMP: {
            int off = (f->ip[0] << 8) | f->ip[1];
            f->ip += 2 + off;
            break;
        }
        case OP_JIF: {
            int off = (f->ip[0] << 8) | f->ip[1];
            f->ip += 2;
            Val v = POP();
            if (v.t != V_BOOL) RTFAIL("E104", "");
            if (v.num == 0) f->ip += off;
            break;
        }
        case OP_LOOP: {
            int off = (f->ip[0] << 8) | f->ip[1];
            f->ip += 2 - off;
            break;
        }
        case OP_REPEAT: {
            int off = (f->ip[0] << 8) | f->ip[1];
            f->ip += 2;
            Val *top = vm->sp - 1;
            if (top->t != V_NUM) RTFAIL("E104", "");
            if (top->num < 1) {
                (void)POP();
                f->ip += off;
            } else {
                top->num -= 1;
            }
            break;
        }
        case OP_CALL: {
            const char *name = f->fn->ch.consts[*f->ip++].str;
            int argc = *f->ip++;
            Val *g = global_find(vm, name);
            if (g && g->t == V_FUNC) {
                Func *fn = g->fn;
                if (argc != fn->arity) RTFAIL("E103", name);
                if (vm->nframes >= FRAMES_MAX) RTFAIL("E109", "");
                Frame *nf = &vm->frames[vm->nframes++];
                nf->fn = fn;
                nf->ip = fn->ch.code;
                nf->base = vm->sp - argc;
                for (int i = fn->arity; i < fn->nlocals; i++)
                    PUSH(((Val){V_UNDEF, 0, NULL, NULL}));
                f = nf;
                break;
            }
            const char *canon = aksa_locale_builtin(vm->loc, name, (int)strlen(name));
            if (!canon) RTFAIL("E102", name);
            Val result;
            if (!builtin_call(vm, LINE(), name, canon, vm->sp - argc, argc, &result))
                return 1;
            vm->sp -= argc;
            PUSH(result);
            break;
        }
        case OP_RET: {
            Val result = POP();
            vm->nframes--;
            if (vm->nframes == 0) return vm->failed;
            vm->sp = f->base;
            f = &vm->frames[vm->nframes - 1];
            PUSH(result);
            break;
        }
        }
    }
#undef LINE
#undef RTFAIL
#undef PUSH
#undef POP
#undef NUMOP
}

/* ---------- entry point ---------- */

int aksa_run(const char *src, const AksaLocale *loc, AksaErrors *errs,
             const AksaHost *host) {
    AksaNode *prog = aksa_parse(src, loc, errs);
    if (errs->count == 0) aksa_check(prog, loc, errs);
    if (errs->count > 0) {
        aksa_ast_free(prog);
        return 1;
    }

    Func *script = calloc(1, sizeof *script);
    script->name = malloc(7);
    strcpy(script->name, "(main)");

    C c = {0};
    c.loc = loc;
    c.errs = errs;
    c.fn = script;
    c.capfuncs = 8;
    c.funcs = malloc(sizeof(Func *) * 8);
    c.funcs[c.nfuncs++] = script;

    compile_block(&c, prog);
    emit(&c, OP_NIL, 0);
    emit(&c, OP_RET, 0);
    aksa_ast_free(prog);

    int rc = c.failed;
    if (!rc) {
        VM *vm = calloc(1, sizeof *vm); /* ~45KB — too big for the WASM stack */
        vm->loc = loc;
        vm->errs = errs;
        vm->out = host && host->out ? host->out : stdout_out;
        vm->in = host && host->in ? host->in : stdin_in;
        vm->yield = host ? host->yield : NULL;
        vm->host = host ? host->host : NULL;
        vm->user = host ? host->user : NULL;
        rc = vm_execute(vm, script);
        for (int i = 0; i < vm->nowned; i++) free(vm->owned[i]);
        free(vm->owned);
        free(vm);
    }

    for (int i = 0; i < c.nfuncs; i++) {
        chunk_free(&c.funcs[i]->ch);
        for (int j = 0; j < c.funcs[i]->nlocals; j++) free(c.funcs[i]->lnames[j]);
        free(c.funcs[i]->name);
        free(c.funcs[i]);
    }
    free(c.funcs);
    return rc;
}
