#ifndef AKSA_AST_H
#define AKSA_AST_H

typedef enum {
    AST_PROGRAM, AST_BLOCK,
    AST_NUM, AST_STR, AST_BOOL, AST_NAME,
    AST_BINOP, AST_UNOP, AST_CALL,
    AST_IF, AST_REPEAT, AST_WHILE, AST_FUNC,
    AST_RETURN, AST_BREAK, AST_ASSIGN,
} AksaAstKind;

typedef struct AksaNode AksaNode;
struct AksaNode {
    AksaAstKind kind;
    int line, col;
    double num;   /* AST_NUM value; AST_BOOL 0/1; AST_ASSIGN 1 = declared with VAR */
    char *str;    /* AST_STR text; AST_NAME/AST_CALL/AST_FUNC/AST_ASSIGN name */
    int op;       /* AST_BINOP/AST_UNOP: the AksaTokKind of the operator */
    AksaNode *a, *b, *c;   /* e.g. IF: a=cond b=then c=else */
    AksaNode **list;       /* PROGRAM/BLOCK statements, CALL args, FUNC params */
    int nlist;
};

AksaNode *aksa_node_new(AksaAstKind kind, int line, int col);
void aksa_node_push(AksaNode *parent, AksaNode *child);
void aksa_ast_free(AksaNode *n);
char *aksa_ast_to_string(const AksaNode *n); /* malloc'd s-expression dump */

#endif
