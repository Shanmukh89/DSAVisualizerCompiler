#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

void generateCode(ASTNode *node);
int hasGenerationError(void);

#endif

