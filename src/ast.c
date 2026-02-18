#include <stdlib.h>
#include <string.h>
#include "ast.h"

static ASTNode *allocateNode(NodeType type) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = type;
    node->name = NULL;
    node->number = 0;
    node->array_values = NULL;
    node->array_count = 0;
    node->left = NULL;
    node->right = NULL;
    node->body = NULL;
    node->cond = NULL;
    node->inc = NULL;
    return node;
}

ASTNode *appendSequence(ASTNode *a, ASTNode *b) {
    if (!a) return b;
    ASTNode *curr = a;
    while (curr->right) curr = curr->right;
    curr->right = b;
    return a;
}

ASTNode *createAssignNode(char *id, ASTNode *value) {
    ASTNode *node = allocateNode(ASSIGN_NODE);
    if (!node) return NULL;
    node->name = _strdup(id);
    node->left = value;
    return node;
}

ASTNode *createLoopNode(ASTNode *init, ASTNode *cond, ASTNode *inc, ASTNode *body) {
    ASTNode *node = allocateNode(LOOP_NODE);
    if (!node) return NULL;
    node->left = init;
    node->cond = cond;
    node->inc = inc;
    node->body = body;
    return node;
}

ASTNode *createIfNode(ASTNode *cond, ASTNode *body) {
    ASTNode *node = allocateNode(IF_NODE);
    if (!node) return NULL;
    node->cond = cond;
    node->body = body;
    return node;
}

ASTNode *createValueNode(char *val) {
    ASTNode *node = allocateNode(VALUE_NODE);
    if (!node) return NULL;
    node->name = _strdup(val);
    return node;
}

ASTNode *createNumberNode(int value) {
    ASTNode *node = allocateNode(NUMBER_NODE);
    if (!node) return NULL;
    node->number = value;
    return node;
}

ASTNode *createArrayDeclNode(char *name, int *values, int count) {
    ASTNode *node = allocateNode(ARRAY_DECL_NODE);
    if (!node) return NULL;
    node->name = _strdup(name);
    if (values && count > 0) {
        node->array_values = (int *)malloc(count * sizeof(int));
        if (node->array_values) {
            for (int i = 0; i < count; i++) {
                node->array_values[i] = values[i];
            }
            node->array_count = count;
        }
    }
    return node;
}

ASTNode *createAlgorithmCallNode(char *algorithm_name, ASTNode *arg) {
    ASTNode *node = allocateNode(ALGORITHM_CALL_NODE);
    if (!node) return NULL;
    node->name = _strdup(algorithm_name);
    node->left = arg;  // Use left to store the argument
    return node;
}

void freeAST(ASTNode *node) {
    if (!node) return;
    freeAST(node->left);
    freeAST(node->right);
    freeAST(node->body);
    freeAST(node->cond);
    freeAST(node->inc);
    if (node->name) free(node->name);
    if (node->array_values) free(node->array_values);
    free(node);
}


