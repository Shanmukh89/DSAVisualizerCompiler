#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h"
#include "ir.h"

static void generateSequence(ASTNode *node);
static void generateBubbleSortSteps(int n, int *array);
static void generateInsertionSortSteps(int n, int *array);
static void generateSelectionSortSteps(int n, int *array);
static void generateMergeSortSteps(int n, int *array);
static void generateQuickSortSteps(int n, int *array);
static void generateStackOperations(ASTNode *operations);
static void generateQueueOperations(ASTNode *operations);
static void generateTreeOperations(ASTNode *operations);
static void generateGraphOperations(ASTNode *operations);
static void collectDSLInfo(ASTNode *node);

static int loop_depth = 0;
static int compare_index = 0;

// Global to store array from DSL
static int dsl_array[256];
static int dsl_array_size = 0;
static char algorithm_to_generate[64] = "";

// Data structure operation tracking
typedef struct OperationNode {
    char operation[32];
    int value;
    int value2;  // For operations like add_edge
    struct OperationNode *next;
} OperationNode;

// Tree node structure for building actual binary search tree
typedef struct TreeNode {
    int value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

static OperationNode *ds_operations = NULL;
static OperationNode *ds_operations_tail = NULL;
static char ds_type[32] = "";  // "stack", "queue", "tree", "graph"

// Error reporting
static int has_error = 0;
static const char *error_message = NULL;
static int semantic_error_line = 0;

// Validation functions
static int isValidAlgorithm(const char *name);
static int isValidDataStructure(const char *name);
static int isValidStackOperation(const char *op);
static int isValidQueueOperation(const char *op);
static int isValidTreeOperation(const char *op);
static int isValidGraphOperation(const char *op);
static void reportError(const char *format, ...);
static void reportErrorAtLine(int line, const char *format, ...);

// Validation functions
static int isValidAlgorithm(const char *name) {
    if (!name || name[0] == '\0') return 0;
    return (strcmp(name, "bubble_sort") == 0 ||
            strcmp(name, "insertion_sort") == 0 ||
            strcmp(name, "selection_sort") == 0 ||
            strcmp(name, "merge_sort") == 0 ||
            strcmp(name, "quick_sort") == 0 ||
            strcmp(name, "quicksort") == 0);
}

static int isValidDataStructure(const char *name) {
    if (!name || name[0] == '\0') return 0;
    return (strcmp(name, "stack") == 0 ||
            strcmp(name, "queue") == 0 ||
            strcmp(name, "tree") == 0 ||
            strcmp(name, "graph") == 0);
}

static int isValidStackOperation(const char *op) {
    if (!op || op[0] == '\0') return 0;
    return (strcmp(op, "push") == 0 ||
            strcmp(op, "pop") == 0 ||
            strcmp(op, "peek") == 0 ||
            strcmp(op, "top") == 0);
}

static int isValidQueueOperation(const char *op) {
    if (!op || op[0] == '\0') return 0;
    return (strcmp(op, "enqueue") == 0 ||
            strcmp(op, "dequeue") == 0);
}

static int isValidTreeOperation(const char *op) {
    if (!op || op[0] == '\0') return 0;
    return (strcmp(op, "insert") == 0 ||
            strcmp(op, "traverse_inorder") == 0 ||
            strcmp(op, "traverse_preorder") == 0 ||
            strcmp(op, "traverse_postorder") == 0);
}

static int isValidGraphOperation(const char *op) {
    if (!op || op[0] == '\0') return 0;
    return (strcmp(op, "add_node") == 0 ||
            strcmp(op, "add_edge") == 0 ||
            strcmp(op, "bfs") == 0 ||
            strcmp(op, "dfs") == 0);
}

static void reportError(const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (semantic_error_line > 0) {
        fprintf(stderr, "Semantic Error at line %d: ", semantic_error_line);
    } else {
        fprintf(stderr, "Semantic Error: ");
    }
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    has_error = 1;
}

static void reportErrorAtLine(int line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "Semantic Error at line %d: ", line);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
    has_error = 1;
    semantic_error_line = line;
}

void generateCode(ASTNode *node) {
    loop_depth = 0;
    compare_index = 0;
    dsl_array_size = 0;
    algorithm_to_generate[0] = '\0';
    ds_type[0] = '\0';
    ds_operations = NULL;
    ds_operations_tail = NULL;
    has_error = 0;
    error_message = NULL;
    
    // Default values
    int default_array[] = {5, 3, 8, 4, 2};
    int array_size = 5;
    strcpy(algorithm_to_generate, "");
    
    // First pass: collect array declarations, algorithm calls, and data structure operations
    collectDSLInfo(node);
    
    // Check if we have data structure operations
    if (ds_operations && ds_type[0] != '\0') {
        // Validate data structure type
        if (!isValidDataStructure(ds_type)) {
            reportError("Unknown data structure: '%s'. Valid options are: stack, queue, tree, graph", ds_type);
            return;
        }
        
        // Validate all operations
        OperationNode *op = ds_operations;
        while (op) {
            int valid = 0;
            if (strcmp(ds_type, "stack") == 0) {
                valid = isValidStackOperation(op->operation);
                if (!valid) {
                    reportError("Invalid stack operation: '%s'. Valid operations are: push, pop, peek, top", op->operation);
                }
            } else if (strcmp(ds_type, "queue") == 0) {
                valid = isValidQueueOperation(op->operation);
                if (!valid) {
                    reportError("Invalid queue operation: '%s'. Valid operations are: enqueue, dequeue", op->operation);
                }
            } else if (strcmp(ds_type, "tree") == 0) {
                valid = isValidTreeOperation(op->operation);
                if (!valid) {
                    reportError("Invalid tree operation: '%s'. Valid operations are: insert, traverse_inorder, traverse_preorder, traverse_postorder", op->operation);
                }
            } else if (strcmp(ds_type, "graph") == 0) {
                valid = isValidGraphOperation(op->operation);
                if (!valid) {
                    reportError("Invalid graph operation: '%s'. Valid operations are: add_node, add_edge, bfs, dfs", op->operation);
                }
            }
            if (has_error) {
                // Free operations list before returning
                while (ds_operations) {
                    OperationNode *next = ds_operations->next;
                    free(ds_operations);
                    ds_operations = next;
                }
                return;
            }
            op = op->next;
        }
        
        // Generate data structure visualization
        if (strcmp(ds_type, "stack") == 0) {
            setAlgorithmName("Stack Operations");
            setDetectedAlgorithm("stack");
            generateStackOperations(node);
        } else if (strcmp(ds_type, "queue") == 0) {
            setAlgorithmName("Queue Operations");
            setDetectedAlgorithm("queue");
            generateQueueOperations(node);
        } else if (strcmp(ds_type, "tree") == 0) {
            setAlgorithmName("Binary Tree Operations");
            setDetectedAlgorithm("tree");
            generateTreeOperations(node);
        } else if (strcmp(ds_type, "graph") == 0) {
            setAlgorithmName("Graph Operations");
            setDetectedAlgorithm("graph");
            generateGraphOperations(node);
        }
        
        // Free operations list
        while (ds_operations) {
            OperationNode *next = ds_operations->next;
            free(ds_operations);
            ds_operations = next;
        }
        return;
    }
    
    // Use DSL array if available, otherwise use default
    if (dsl_array_size > 0) {
        setArray(dsl_array, dsl_array_size);
        array_size = dsl_array_size;
    } else {
        setArray(default_array, array_size);
        for (int i = 0; i < array_size; i++) {
            dsl_array[i] = default_array[i];
        }
        dsl_array_size = array_size;
    }
    
    // Determine algorithm name and display name
    char display_name[64] = "Algorithm";
    if (algorithm_to_generate[0] != '\0') {
        // Validate algorithm name
        if (!isValidAlgorithm(algorithm_to_generate)) {
            reportError("Unknown algorithm: '%s'. Valid algorithms are: bubble_sort, insertion_sort, selection_sort, merge_sort, quick_sort", algorithm_to_generate);
            fprintf(stderr, "Did you mean: bubble_sort?\n");
            return;
        }
        
        // Map algorithm names to display names
        if (strcmp(algorithm_to_generate, "bubble_sort") == 0) {
            strcpy(display_name, "Bubble Sort");
        } else if (strcmp(algorithm_to_generate, "insertion_sort") == 0) {
            strcpy(display_name, "Insertion Sort");
        } else if (strcmp(algorithm_to_generate, "selection_sort") == 0) {
            strcpy(display_name, "Selection Sort");
        } else if (strcmp(algorithm_to_generate, "merge_sort") == 0) {
            strcpy(display_name, "Merge Sort");
        } else if (strcmp(algorithm_to_generate, "quick_sort") == 0 || strcmp(algorithm_to_generate, "quicksort") == 0) {
            strcpy(display_name, "Quick Sort");
        }
        setAlgorithmName(display_name);
        setDetectedAlgorithm(algorithm_to_generate);
    } else {
        strcpy(display_name, "Bubble Sort");
        setAlgorithmName(display_name);
        setDetectedAlgorithm("bubble_sort");
    }
    
    int *array_to_use = dsl_array_size > 0 ? dsl_array : default_array;
    
    // Generate steps based on detected algorithm
    if (strcmp(algorithm_to_generate, "bubble_sort") == 0 || algorithm_to_generate[0] == '\0') {
        generateBubbleSortSteps(array_size, array_to_use);
    } else if (strcmp(algorithm_to_generate, "insertion_sort") == 0) {
        generateInsertionSortSteps(array_size, array_to_use);
    } else if (strcmp(algorithm_to_generate, "selection_sort") == 0) {
        generateSelectionSortSteps(array_size, array_to_use);
    } else if (strcmp(algorithm_to_generate, "merge_sort") == 0) {
        generateMergeSortSteps(array_size, array_to_use);
    } else if (strcmp(algorithm_to_generate, "quick_sort") == 0 || strcmp(algorithm_to_generate, "quicksort") == 0) {
        generateQuickSortSteps(array_size, array_to_use);
    }
}

// Get error status
int hasGenerationError(void) {
    return has_error;
}

static void addOperation(const char *op, int val, int val2) {
    OperationNode *node = (OperationNode *)calloc(1, sizeof(OperationNode));
    if (!node) return;
    strncpy(node->operation, op, sizeof(node->operation) - 1);
    node->value = val;
    node->value2 = val2;
    node->next = NULL;
    if (!ds_operations) {
        ds_operations = ds_operations_tail = node;
    } else {
        ds_operations_tail->next = node;
        ds_operations_tail = node;
    }
}

static int extractNumberFromAST(ASTNode *node) {
    if (!node) return 0;
    if (node->type == NUMBER_NODE) {
        return node->number;
    }
    if (node->type == VALUE_NODE && node->name) {
        // Try to parse as number
        return atoi(node->name);
    }
    return 0;
}

static void collectDSLInfo(ASTNode *node) {
    if (!node) return;
    
    static int node_count = 0;  // Track node position as proxy for line
    
    for (ASTNode *curr = node; curr != NULL; curr = curr->right) {
        node_count++;
        
        switch (curr->type) {
            case ARRAY_DECL_NODE:
                // Extract array values
                if (curr->array_values && curr->array_count > 0) {
                    dsl_array_size = curr->array_count;
                    for (int i = 0; i < curr->array_count && i < 256; i++) {
                        dsl_array[i] = curr->array_values[i];
                    }
                }
                // Validate array declaration
                if (!curr->name || curr->name[0] == '\0') {
                    reportErrorAtLine(node_count, "Invalid array declaration: missing array name");
                }
                break;
            case ALGORITHM_CALL_NODE:
                // Check if it's a data structure operation (obj.method format)
                if (curr->name && strchr(curr->name, '.')) {
                    // Parse as data_structure.operation
                    char ds_name[64];
                    char op_name[64];
                    if (sscanf(curr->name, "%[^.].%s", ds_name, op_name) == 2) {
                        strncpy(ds_type, ds_name, sizeof(ds_type) - 1);
                        
                        // Extract argument values (arguments are linked via 'right')
                        int val = 0, val2 = 0;
                        ASTNode *arg = curr->left;
                        if (arg) {
                            val = extractNumberFromAST(arg);
                            // Second argument if exists
                            if (arg->right) {
                                val2 = extractNumberFromAST(arg->right);
                            }
                        }
                        
                        addOperation(op_name, val, val2);
                    } else {
                        reportErrorAtLine(node_count, "Invalid data structure operation format: expected 'structure.operation(args)'");
                    }
                } else if (curr->name) {
                    // Regular algorithm call - validate it has an argument
                    strncpy(algorithm_to_generate, curr->name, sizeof(algorithm_to_generate) - 1);
                    algorithm_to_generate[sizeof(algorithm_to_generate) - 1] = '\0';
                    
                    // Validate that array exists for algorithm calls
                    if (dsl_array_size == 0 && curr->left == NULL) {
                        reportErrorAtLine(node_count, "Algorithm '%s' called without array argument. Use: %s(array_var)", 
                                        curr->name, curr->name);
                    }
                } else {
                    reportErrorAtLine(node_count, "Invalid function call: missing function name");
                }
                break;
            case ASSIGN_NODE:
                // Validate assignment
                if (!curr->name || curr->name[0] == '\0') {
                    reportErrorAtLine(node_count, "Invalid assignment: missing variable name");
                }
                break;
            default:
                // Recursively process children
                if (curr->body) collectDSLInfo(curr->body);
                if (curr->left) collectDSLInfo(curr->left);
                break;
        }
        
        if (has_error) break;  // Stop on first error
    }
}

static void generateBubbleSortSteps(int n, int *array) {
    int working_array[256];
    for (int k = 0; k < n; k++) {
        working_array[k] = array[k];
    }
    
    for (int pass = 0; pass < n - 1; pass++) {
        for (int i = 0; i < n - pass - 1; i++) {
            int j = i + 1;
            int compare_indices[2] = {i, j};
            addIRWithIndices("compare", "", compare_indices, 2);
            
            if (working_array[i] > working_array[j]) {
                addIRWithIndices("swap", "", compare_indices, 2);
                int temp = working_array[i];
                working_array[i] = working_array[j];
                working_array[j] = temp;
            } else {
                addIRWithIndices("no_swap", "", compare_indices, 2);
            }
        }
        int sorted_index = n - pass - 1;
        addIRWithIndices("mark_sorted", "", &sorted_index, 1);
    }
}

static void generateInsertionSortSteps(int n, int *array) {
    int working_array[256];
    for (int k = 0; k < n; k++) {
        working_array[k] = array[k];
    }
    
    for (int i = 1; i < n; i++) {
        int key = working_array[i];
        int j = i - 1;
        
        // Show current element being inserted
        int current_indices[2] = {i, j};
        addIRWithIndices("compare", "", current_indices, 2);
        
        while (j >= 0 && working_array[j] > key) {
            int compare_indices[2] = {j, j + 1};
            addIRWithIndices("compare", "", compare_indices, 2);
            
            // Shift element
            working_array[j + 1] = working_array[j];
            int shift_indices[2] = {j, j + 1};
            addIRWithIndices("swap", "", shift_indices, 2);
            j--;
        }
        
        working_array[j + 1] = key;
        
        // Mark sorted position
        int sorted_index = i;
        addIRWithIndices("mark_sorted", "", &sorted_index, 1);
    }
}

static void generateSelectionSortSteps(int n, int *array) {
    int working_array[256];
    for (int k = 0; k < n; k++) {
        working_array[k] = array[k];
    }
    
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        
        // Find minimum element
        for (int j = i + 1; j < n; j++) {
            int compare_indices[2] = {min_idx, j};
            addIRWithIndices("compare", "", compare_indices, 2);
            
            if (working_array[j] < working_array[min_idx]) {
                min_idx = j;
                addIRWithIndices("no_swap", "", compare_indices, 2);
            } else {
                addIRWithIndices("no_swap", "", compare_indices, 2);
            }
        }
        
        // Swap minimum with current position
        if (min_idx != i) {
            int swap_indices[2] = {i, min_idx};
            addIRWithIndices("swap", "", swap_indices, 2);
            int temp = working_array[i];
            working_array[i] = working_array[min_idx];
            working_array[min_idx] = temp;
        }
        
        // Mark sorted position
        addIRWithIndices("mark_sorted", "", &i, 1);
    }
}

static void generateMergeSortSteps(int n, int *array) {
    int working_array[256];
    for (int k = 0; k < n; k++) {
        working_array[k] = array[k];
    }
    
    // Merge sort visualization - top-down approach
    // For visualization, we'll show the divide and merge steps
    int step = 1;
    while (step < n) {
        for (int left = 0; left < n - step; left += 2 * step) {
            int mid = left + step - 1;
            int right = (left + 2 * step - 1 < n) ? (left + 2 * step - 1) : (n - 1);
            
            // Merge the two halves
            int temp[256];
            int i = left, j = mid + 1, k = 0;
            
            while (i <= mid && j <= right) {
                int compare_indices[2] = {i, j};
                addIRWithIndices("compare", "", compare_indices, 2);
                
                if (working_array[i] <= working_array[j]) {
                    temp[k++] = working_array[i++];
                    addIRWithIndices("no_swap", "", compare_indices, 2);
                } else {
                    temp[k++] = working_array[j++];
                    addIRWithIndices("swap", "", compare_indices, 2);
                }
            }
            
            while (i <= mid) {
                temp[k++] = working_array[i++];
            }
            
            while (j <= right) {
                temp[k++] = working_array[j++];
            }
            
            // Copy merged array back
            for (i = left, k = 0; i <= right; i++, k++) {
                working_array[i] = temp[k];
            }
        }
        step *= 2;
    }
}

static void generateQuickSortSteps(int n, int *array) {
    int working_array[256];
    for (int k = 0; k < n; k++) {
        working_array[k] = array[k];
    }
    
    // Quick sort visualization - using iterative approach with stack
    typedef struct { int low; int high; } Range;
    Range stack[256];
    int top = 0;
    
    stack[top].low = 0;
    stack[top].high = n - 1;
    top++;
    
    while (top > 0) {
        top--;
        int low = stack[top].low;
        int high = stack[top].high;
        
        if (low < high) {
            // Partition
            int pivot = working_array[high];
            int i = low - 1;
            
            // Show pivot selection
            int pivot_index = high;
            addIRWithIndices("compare", "", &pivot_index, 1);
            
            for (int j = low; j < high; j++) {
                int compare_indices[2] = {j, high};
                addIRWithIndices("compare", "", compare_indices, 2);
                
                if (working_array[j] < pivot) {
                    i++;
                    if (i != j) {
                        int swap_indices[2] = {i, j};
                        addIRWithIndices("swap", "", swap_indices, 2);
                        int temp = working_array[i];
                        working_array[i] = working_array[j];
                        working_array[j] = temp;
                    } else {
                        addIRWithIndices("no_swap", "", compare_indices, 2);
                    }
                } else {
                    addIRWithIndices("no_swap", "", compare_indices, 2);
                }
            }
            
            // Place pivot in correct position
            int swap_indices[2] = {i + 1, high};
            addIRWithIndices("swap", "", swap_indices, 2);
            int temp = working_array[i + 1];
            working_array[i + 1] = working_array[high];
            working_array[high] = temp;
            
            int pivot_pos = i + 1;
            addIRWithIndices("mark_sorted", "", &pivot_pos, 1);
            
            // Push sub-arrays to stack
            if (i + 1 + 1 < high) {
                stack[top].low = i + 2;
                stack[top].high = high;
                top++;
            }
            if (low < i + 1 - 1) {
                stack[top].low = low;
                stack[top].high = i;
                top++;
            }
        }
    }
}

static void generateStackOperations(ASTNode *operations) {
    // Stack visualization: LIFO operations
    int stack[256];
    int top = -1;
    int step = 0;
    
    OperationNode *op = ds_operations;
    while (op) {
        if (strcmp(op->operation, "push") == 0) {
            top++;
            stack[top] = op->value;
            // Push action: element added at top
            int indices[2] = {top, op->value};
            addIRWithIndices("push", "", indices, 2);
        } else if (strcmp(op->operation, "pop") == 0) {
            if (top >= 0) {
                int val = stack[top];
                int indices[2] = {top, val};
                addIRWithIndices("pop", "", indices, 2);
                top--;
            }
        } else if (strcmp(op->operation, "peek") == 0 || strcmp(op->operation, "top") == 0) {
            if (top >= 0) {
                int indices[1] = {top};
                addIRWithIndices("peek", "", indices, 1);
            }
        }
        op = op->next;
    }
}

static void generateQueueOperations(ASTNode *operations) {
    // Queue visualization: FIFO operations
    int queue[256];
    int front = 0;
    int rear = -1;
    int size = 0;
    
    OperationNode *op = ds_operations;
    while (op) {
        if (strcmp(op->operation, "enqueue") == 0) {
            rear++;
            queue[rear] = op->value;
            size++;
            int indices[2] = {rear, op->value};
            addIRWithIndices("enqueue", "", indices, 2);
        } else if (strcmp(op->operation, "dequeue") == 0) {
            if (size > 0) {
                int val = queue[front];
                int indices[2] = {front, val};
                addIRWithIndices("dequeue", "", indices, 2);
                front++;
                size--;
            }
        }
        op = op->next;
    }
}

// Helper function to insert into BST and return tree structure info
static TreeNode* insertTreeNode(TreeNode *root, int value, int *parent_val, int *is_left) {
    if (root == NULL) {
        TreeNode *new_node = (TreeNode *)malloc(sizeof(TreeNode));
        new_node->value = value;
        new_node->left = NULL;
        new_node->right = NULL;
        return new_node;
    }
    
    *parent_val = root->value;
    if (value < root->value) {
        *is_left = 1;
        root->left = insertTreeNode(root->left, value, parent_val, is_left);
    } else {
        *is_left = 0;
        root->right = insertTreeNode(root->right, value, parent_val, is_left);
    }
    return root;
}

// Helper function for inorder traversal
static void inorderTraversal(TreeNode *root, int *result, int *index) {
    if (root == NULL) return;
    inorderTraversal(root->left, result, index);
    result[(*index)++] = root->value;
    inorderTraversal(root->right, result, index);
}

// Helper function for preorder traversal
static void preorderTraversal(TreeNode *root, int *result, int *index) {
    if (root == NULL) return;
    result[(*index)++] = root->value;
    preorderTraversal(root->left, result, index);
    preorderTraversal(root->right, result, index);
}

// Helper function for postorder traversal
static void postorderTraversal(TreeNode *root, int *result, int *index) {
    if (root == NULL) return;
    postorderTraversal(root->left, result, index);
    postorderTraversal(root->right, result, index);
    result[(*index)++] = root->value;
}

// Helper to free tree
static void freeTree(TreeNode *root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

static void generateTreeOperations(ASTNode *operations) {
    // Binary tree operations - build actual BST
    TreeNode *root = NULL;
    int tree_values[256];
    int tree_size = 0;
    
    OperationNode *op = ds_operations;
    while (op) {
        if (strcmp(op->operation, "insert") == 0) {
            int parent_val = -1;
            int is_left = -1;
            if (root == NULL) {
                // Root insertion
                int indices[4] = {op->value, -1, -1, 0}; // value, parent (none), side (none), depth
                addIRWithIndices("insert", "", indices, 4);
            } else {
                // Find where to insert by simulating
                int temp_parent = -1, temp_side = -1;
                TreeNode *temp = root;
                while (1) {
                    if (op->value < temp->value) {
                        if (temp->left == NULL) {
                            temp_parent = temp->value;
                            temp_side = 1;
                            break;
                        }
                        temp = temp->left;
                    } else {
                        if (temp->right == NULL) {
                            temp_parent = temp->value;
                            temp_side = 0;
                            break;
                        }
                        temp = temp->right;
                    }
                }
                int indices[4] = {op->value, temp_parent, temp_side, 0};
                addIRWithIndices("insert", "", indices, 4);
            }
            // Actually insert into tree
            root = insertTreeNode(root, op->value, &parent_val, &is_left);
            tree_values[tree_size++] = op->value;
        } else if (strcmp(op->operation, "traverse_inorder") == 0) {
            int result[256];
            int count = 0;
            inorderTraversal(root, result, &count);
            for (int i = 0; i < count; i++) {
                int indices[1] = {result[i]};
                addIRWithIndices("visit", "", indices, 1);
            }
        } else if (strcmp(op->operation, "traverse_preorder") == 0) {
            int result[256];
            int count = 0;
            preorderTraversal(root, result, &count);
            for (int i = 0; i < count; i++) {
                int indices[1] = {result[i]};
                addIRWithIndices("visit", "", indices, 1);
            }
        } else if (strcmp(op->operation, "traverse_postorder") == 0) {
            int result[256];
            int count = 0;
            postorderTraversal(root, result, &count);
            for (int i = 0; i < count; i++) {
                int indices[1] = {result[i]};
                addIRWithIndices("visit", "", indices, 1);
            }
        }
        op = op->next;
    }
    
    // Store tree values in array for visualization
    if (tree_size > 0) {
        setArray(tree_values, tree_size);
    }
    
    freeTree(root);
}

static void generateGraphOperations(ASTNode *operations) {
    // Graph operations: nodes, edges, and traversals
    int nodes[256];
    int node_count = 0;
    int edges[256][2];
    int edge_count = 0;
    
    OperationNode *op = ds_operations;
    while (op) {
        if (strcmp(op->operation, "add_node") == 0) {
            nodes[node_count++] = op->value;
            int indices[1] = {op->value};
            addIRWithIndices("add_node", "", indices, 1);
        } else if (strcmp(op->operation, "add_edge") == 0) {
            edges[edge_count][0] = op->value;
            edges[edge_count][1] = op->value2;
            edge_count++;
            int indices[2] = {op->value, op->value2};
            addIRWithIndices("add_edge", "", indices, 2);
        } else if (strcmp(op->operation, "bfs") == 0) {
            // BFS traversal starting from op->value
            int start = op->value;
            int indices[1] = {start};
            addIRWithIndices("bfs_start", "", indices, 1);
            
            // Simulate BFS: visit nodes level by level
            int visited[256] = {0};
            int queue[256];
            int q_front = 0, q_rear = -1;
            
            visited[start] = 1;
            queue[++q_rear] = start;
            addIRWithIndices("visit", "", indices, 1);
            
            while (q_front <= q_rear) {
                int current = queue[q_front++];
                
                // Visit neighbors
                for (int i = 0; i < edge_count; i++) {
                    if (edges[i][0] == current && !visited[edges[i][1]]) {
                        visited[edges[i][1]] = 1;
                        queue[++q_rear] = edges[i][1];
                        int visit_indices[1] = {edges[i][1]};
                        addIRWithIndices("visit", "", visit_indices, 1);
                    } else if (edges[i][1] == current && !visited[edges[i][0]]) {
                        visited[edges[i][0]] = 1;
                        queue[++q_rear] = edges[i][0];
                        int visit_indices[1] = {edges[i][0]};
                        addIRWithIndices("visit", "", visit_indices, 1);
                    }
                }
            }
        } else if (strcmp(op->operation, "dfs") == 0) {
            // DFS traversal starting from op->value
            int start = op->value;
            int indices[1] = {start};
            addIRWithIndices("dfs_start", "", indices, 1);
            
            // Simulate DFS using recursive approach (iterative with stack)
            int visited[256] = {0};
            int stack[256];
            int top = -1;
            
            stack[++top] = start;
            
            while (top >= 0) {
                int current = stack[top--];
                if (!visited[current]) {
                    visited[current] = 1;
                    int visit_indices[1] = {current};
                    addIRWithIndices("visit", "", visit_indices, 1);
                    
                    // Push neighbors
                    for (int i = 0; i < edge_count; i++) {
                        if (edges[i][0] == current && !visited[edges[i][1]]) {
                            stack[++top] = edges[i][1];
                        } else if (edges[i][1] == current && !visited[edges[i][0]]) {
                            stack[++top] = edges[i][0];
                        }
                    }
                }
            }
        }
        op = op->next;
    }
}

static void extractExprString(ASTNode *node, char *buffer, size_t bufSize) {
    if (!node) {
        buffer[0] = '\0';
        return;
    }
    switch (node->type) {
        case VALUE_NODE:
            if (node->name) {
                strncpy(buffer, node->name, bufSize - 1);
                buffer[bufSize - 1] = '\0';
            }
            break;
        case NUMBER_NODE:
            snprintf(buffer, bufSize, "%d", node->number);
            break;
        default:
            buffer[0] = '\0';
            break;
    }
}

static void generateSequence(ASTNode *node) {
    for (ASTNode *curr = node; curr != NULL; curr = curr->right) {
        switch (curr->type) {
            case ASSIGN_NODE: {
                // Check if it's a swap call
                if (curr->name && (strcmp(curr->name, "swap") == 0)) {
                    // Generate swap action with current indices
                    // Note: compare_index is managed by IF_NODE
                    int indices[2] = {compare_index, compare_index + 1};
                    addIRWithIndices("swap", "", indices, 2);
                }
                break;
            }
            case LOOP_NODE: {
                loop_depth++;
                compare_index = 0;  // Reset for new loop iteration
                if (curr->body) generateSequence(curr->body);
                
                // After loop ends, mark last element as sorted
                if (loop_depth > 0) {
                    // Use array length - loop_depth as the sorted index
                    int index = 4 - loop_depth;  // Simplified - would need actual array size
                    if (index >= 0) {
                        addIRWithIndices("mark_sorted", "", &index, 1);
                    }
                }
                
                loop_depth--;
                break;
            }
            case IF_NODE: {
                // IF condition - generate compare action
                if (loop_depth > 0) {
                    int indices[2] = {compare_index, compare_index + 1};
                    addIRWithIndices("compare", "", indices, 2);
                }
                
                // Process body (which will contain swap or no_swap)
                int had_swap = 0;
                if (curr->body) {
                    // Check if body contains swap
                    for (ASTNode *body_node = curr->body; body_node; body_node = body_node->right) {
                        if (body_node->type == ASSIGN_NODE && 
                            body_node->name && strcmp(body_node->name, "swap") == 0) {
                            had_swap = 1;
                break;
                        }
                    }
                    generateSequence(curr->body);
                }
                
                // If no swap happened, add no_swap
                if (!had_swap && loop_depth > 0) {
                    int indices[2] = {compare_index, compare_index + 1};
                    addIRWithIndices("no_swap", "", indices, 2);
                }
                
                // Increment compare_index after processing this if block
                if (loop_depth > 0) {
                    compare_index++;
                }
                break;
            }
            case VALUE_NODE:
            case NUMBER_NODE:
            case SEQ_NODE:
            default:
                // No direct IR action
                break;
        }
    }
}


