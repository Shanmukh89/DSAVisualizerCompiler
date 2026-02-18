#include <stdlib.h>
#include <string.h>
#include "ast.h"

// Helper function to build array from number list
int* buildArrayFromList(ASTNode *node, int *count) {
    if (!node) {
        *count = 0;
        return NULL;
    }
    
    // For now, we'll use a simple approach - collect numbers
    int values[256];
    int idx = 0;
    
    // Traverse the list if it's structured as a sequence
    // For simplicity, assume node contains array_values
    if (node->array_values && node->array_count > 0) {
        int *arr = (int *)malloc(node->array_count * sizeof(int));
        if (arr) {
            for (int i = 0; i < node->array_count; i++) {
                arr[i] = node->array_values[i];
            }
            *count = node->array_count;
            return arr;
        }
    }
    
    *count = 0;
    return NULL;
}

