#ifndef AST_H
#define AST_H

typedef enum {
    ASSIGN_NODE,
    LOOP_NODE,
    IF_NODE,
    VALUE_NODE,
    NUMBER_NODE,
    SEQ_NODE,
    ARRAY_DECL_NODE,
    ALGORITHM_CALL_NODE
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char *name;              // For identifiers or string values
    int number;              // For numeric literals
    int *array_values;       // For array declaration values
    int array_count;         // Number of elements in array
    struct ASTNode *left;    // Generic child (e.g., value/init)
    struct ASTNode *right;   // Next in sequence or generic child
    struct ASTNode *body;    // Body for loop/if
    struct ASTNode *cond;    // Condition for loop/if
    struct ASTNode *inc;     // Increment for loop
} ASTNode;

ASTNode *appendSequence(ASTNode *a, ASTNode *b);

ASTNode *createAssignNode(char *id, ASTNode *value);
ASTNode *createLoopNode(ASTNode *init, ASTNode *cond, ASTNode *inc, ASTNode *body);
ASTNode *createIfNode(ASTNode *cond, ASTNode *body);
ASTNode *createValueNode(char *val);
ASTNode *createNumberNode(int value);
ASTNode *createArrayDeclNode(char *name, int *values, int count);
ASTNode *createAlgorithmCallNode(char *algorithm_name, ASTNode *arg);

void freeAST(ASTNode *node);

#endif


