#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    #define CLEAR_SCREEN() system("cls")
    #define GETCHAR() _getch()
#else
    #include <unistd.h>
    #include <termios.h>
    #define CLEAR_SCREEN() system("clear")
    #define GETCHAR() getchar()
#endif

// Global flag for clean exit
static volatile int should_exit = 0;

// Algorithm state tracking
static int current_pass = 0;
static int current_comparison = 0;
static int total_swaps = 0;
static int total_comparisons = 0;

// Data structure operation tracking
static int stack_pushes = 0;
static int stack_pops = 0;
static int queue_enqueues = 0;
static int queue_dequeues = 0;
static int tree_inserts = 0;
static int tree_visits = 0;
static int graph_nodes = 0;
static int graph_edges = 0;

// Signal handler for Ctrl+C
#ifdef _WIN32
BOOL WINAPI ConsoleHandler(DWORD dwType) {
    switch (dwType) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            should_exit = 1;
            printf("\n\n[Interrupted] Exiting visualization...\n");
            return TRUE;
        default:
            return FALSE;
    }
}
#else
void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        should_exit = 1;
        printf("\n\n[Interrupted] Exiting visualization...\n");
    }
}
#endif

// ANSI color codes (Windows 10+ supports these)
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

typedef struct Step {
    char action[32];
    int indices[4];
    int index_count;
    struct Step *next;
} Step;

typedef struct {
    char algorithm[64];
    int array[256];
    int array_size;
    Step *steps;
} JSONData;

// Helper function to extract string from JSON line
static int extractString(const char *line, const char *key, char *output, size_t max_len) {
    char search_str[128];
    snprintf(search_str, sizeof(search_str), "\"%s\"", key);
    char *pos = strstr(line, search_str);
    if (!pos) return 0;
    
    pos = strchr(pos, ':');
    if (!pos) return 0;
    pos++;
    
    while (*pos && isspace(*pos)) pos++;
    if (*pos == '"') {
        pos++;
        char *end = strchr(pos, '"');
        if (!end) return 0;
        size_t len = end - pos;
        if (len >= max_len) len = max_len - 1;
        strncpy(output, pos, len);
        output[len] = '\0';
        return 1;
    }
    return 0;
}

// Helper function to extract array from JSON
static int extractArray(const char *line, int *arr, int max_size, int *actual_size) {
    char *pos = strchr(line, '[');
    if (!pos) return 0;
    pos++;
    
    *actual_size = 0;
    while (*pos && *pos != ']' && *actual_size < max_size) {
        while (*pos && (isspace(*pos) || *pos == ',')) pos++;
        if (*pos == ']') break;
        
        int value;
        char *end;
        value = (int)strtol(pos, &end, 10);
        if (end == pos) break;
        
        arr[*actual_size] = value;
        (*actual_size)++;
        pos = end;
    }
    return (*actual_size > 0);
}

// Helper function to extract indices
static int extractIndices(const char *line, int *indices, int *count) {
    *count = 0;
    
    char *indices_pos = strstr(line, "\"indices\"");
    if (indices_pos) {
        char *arr_start = strchr(indices_pos, '[');
        if (arr_start) {
            arr_start++;
            int idx = 0;
            while (*arr_start && *arr_start != ']' && idx < 4) {
                while (*arr_start && (isspace(*arr_start) || *arr_start == ',')) arr_start++;
                if (*arr_start == ']') break;
                
                int value;
                char *end;
                value = (int)strtol(arr_start, &end, 10);
                if (end == arr_start) break;
                
                indices[idx++] = value;
                arr_start = end;
            }
            *count = idx;
            return 1;
        }
    }
    
    char *index_pos = strstr(line, "\"index\"");
    if (index_pos) {
        index_pos = strchr(index_pos, ':');
        if (index_pos) {
            index_pos++;
            while (*index_pos && isspace(*index_pos)) index_pos++;
            int value;
            char *end;
            value = (int)strtol(index_pos, &end, 10);
            if (end != index_pos) {
                indices[0] = value;
                *count = 1;
                return 1;
            }
        }
    }
    
    return 0;
}

// JSON parser (improved version based on json_to_js.c)
static JSONData *parseJSON(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return NULL;
    
    JSONData *data = (JSONData *)calloc(1, sizeof(JSONData));
    if (!data) { fclose(fp); return NULL; }
    
    strcpy(data->algorithm, "Algorithm");
    data->array_size = 0;
    data->steps = NULL;
    
    // Read entire file
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer) { fclose(fp); free(data); return NULL; }
    fread(buffer, 1, file_size, fp);
    buffer[file_size] = '\0';
    fclose(fp);
    
    // Extract algorithm name
    char alg_name[64];
    if (extractString(buffer, "algorithm", alg_name, sizeof(alg_name))) {
        strncpy(data->algorithm, alg_name, sizeof(data->algorithm) - 1);
    }
    
    // Extract array
    if (strstr(buffer, "\"array\"")) {
        extractArray(buffer, data->array, 256, &data->array_size);
    }
    
    // Extract steps
    char *steps_start = strstr(buffer, "\"steps\"");
    if (steps_start) {
        steps_start = strchr(steps_start, '[');
        if (steps_start) {
            steps_start++;
            Step *step_tail = NULL;
            
            while (*steps_start) {
                // Find next step object
                char *obj_start = strchr(steps_start, '{');
                if (!obj_start) break;
                
                char *obj_end = strchr(obj_start, '}');
                if (!obj_end) break;
                
                int obj_len = obj_end - obj_start;
                char *step_obj = (char *)malloc(obj_len + 1);
                if (step_obj) {
                    strncpy(step_obj, obj_start, obj_len);
                    step_obj[obj_len] = '\0';
                    
                    Step *step = (Step *)calloc(1, sizeof(Step));
                    if (step) {
                        char action[64];
                        if (extractString(step_obj, "action", action, sizeof(action))) {
                            strncpy(step->action, action, sizeof(step->action) - 1);
                        }
                        
                        if (!extractIndices(step_obj, step->indices, &step->index_count)) {
                            step->index_count = 0;
                        }
                        
                        step->next = NULL;
                        if (!data->steps) {
                            data->steps = step_tail = step;
                        } else {
                            step_tail->next = step;
                            step_tail = step;
                        }
                    }
                    free(step_obj);
                }
                
                steps_start = obj_end + 1;
            }
        }
    }
    
    free(buffer);
    return data;
}

// Display array with highlighting (using simple ASCII for compatibility)
static void displayArray(int *arr, int size, int highlight1, int highlight2, int sorted) {
    (void)sorted; // parameter retained for compatibility but intentionally unused
    printf("\n");
    printf("+");
    for (int i = 0; i < size; i++) {
        printf("-------");
        if (i < size - 1) printf("+");
    }
    printf("+\n");
    
    printf("|");
    for (int i = 0; i < size; i++) {
        printf(" [%3d] ", arr[i]);
        printf("|");
    }
    printf("\n");
    
    printf("+");
    for (int i = 0; i < size; i++) {
        printf("-------");
        if (i < size - 1) printf("+");
    }
    printf("+\n");
    
    // Show indices with highlighting
    printf(" ");
    for (int i = 0; i < size; i++) {
        if (i == highlight1 || i == highlight2) {
            printf(" *%2d*  ", i);  // Highlighted indices with *
        } else {
            printf("  %2d   ", i);
        }
    }
    printf("\n\n");
}

// Display stack as vertical structure (LIFO)
static void displayStack(int *stack, int top, int highlight_pos) {
    printf("\n  STACK (LIFO - Last In First Out)\n");
    printf("  +---+\n");
    if (top < 0) {
        printf("  |   |  <-- TOP (empty)\n");
    } else {
        for (int i = top; i >= 0; i--) {
            if (i == top) {
                printf("  |%3d|  <-- TOP\n", stack[i]);
            } else {
                printf("  |%3d|\n", stack[i]);
            }
            printf("  +---+\n");
        }
    }
    if (top >= 0 && highlight_pos == top) {
        printf("  ^^^^^ (highlighted)\n");
    }
    printf("\n");
}

// Display queue as horizontal structure (FIFO)
static void displayQueue(int *queue, int front, int rear, int highlight_pos) {
    printf("\n  QUEUE (FIFO - First In First Out)\n");
    if (front > rear) {
        printf("  [EMPTY]\n\n");
        return;
    }
    
    printf("  Front --> ");
    for (int i = front; i <= rear; i++) {
        if (i == highlight_pos) {
            printf("[%3d]* ", queue[i]);
        } else {
            printf("[%3d] ", queue[i]);
        }
    }
    printf("<-- Rear\n");
    printf("  ");
    for (int i = front; i <= rear; i++) {
        if (i == front) {
            printf("  ^^^   ");
        } else if (i == rear) {
            printf("  ^^^   ");
        } else {
            printf("        ");
        }
    }
    printf("\n");
    printf("  ");
    for (int i = front; i <= rear; i++) {
        if (i == front) {
            printf("Front  ");
        } else if (i == rear) {
            printf("Rear   ");
        } else {
            printf("       ");
        }
    }
    printf("\n\n");
}

// Tree node structure for visualization
typedef struct TreeVizNode {
    int value;
    int parent_val;
    int is_left;  // 1 = left child, 0 = right child, -1 = root
    struct TreeVizNode *left;
    struct TreeVizNode *right;
} TreeVizNode;

// Build tree structure from insert operations
static TreeVizNode* buildTreeFromSteps(Step *steps, int *node_count) {
    TreeVizNode *root = NULL;
    TreeVizNode *nodes[256] = {NULL};
    *node_count = 0;
    
    Step *step = steps;
    while (step) {
        if (strcmp(step->action, "insert") == 0 && step->index_count >= 4) {
            int value = step->indices[0];
            int parent_val = step->indices[1];
            int is_left = step->indices[2];
            
            TreeVizNode *new_node = (TreeVizNode *)calloc(1, sizeof(TreeVizNode));
            new_node->value = value;
            new_node->parent_val = parent_val;
            new_node->is_left = is_left;
            new_node->left = NULL;
            new_node->right = NULL;
            
            if (parent_val == -1) {
                root = new_node;
            } else {
                // Find parent node
                for (int i = 0; i < *node_count; i++) {
                    if (nodes[i] && nodes[i]->value == parent_val) {
                        if (is_left) {
                            nodes[i]->left = new_node;
                        } else {
                            nodes[i]->right = new_node;
                        }
                        break;
                    }
                }
            }
            nodes[*node_count] = new_node;
            (*node_count)++;
        }
        step = step->next;
    }
    
    return root;
}

// Calculate tree height for display
static int treeHeight(TreeVizNode *root) {
    if (root == NULL) return 0;
    int left_h = treeHeight(root->left);
    int right_h = treeHeight(root->right);
    return (left_h > right_h ? left_h : right_h) + 1;
}

// (unused) legacy vertical tree printer removed

// Display tree structure in a clean vertical format (only up to current step)
static void displayTree(Step *steps, Step *current_step) {
    // Build tree only from steps up to current_step
    Step *s = steps;
    int node_count = 0;
    TreeVizNode *root = NULL;
    
    // Build tree incrementally up to current step
    // If current_step is NULL, show all steps
    while (s && (current_step == NULL || s != current_step->next)) {
        if (strcmp(s->action, "insert") == 0 && s->index_count >= 4) {
            int value = s->indices[0];
            int parent_val = s->indices[1];
            int is_left = s->indices[2];
            
            if (parent_val == -1) {
                // Root node
                root = (TreeVizNode *)malloc(sizeof(TreeVizNode));
                root->value = value;
                root->parent_val = -1;
                root->is_left = -1;
                root->left = NULL;
                root->right = NULL;
                node_count++;
            } else {
                // Find parent node
                TreeVizNode *parent = NULL;
                TreeVizNode **queue = (TreeVizNode **)malloc(sizeof(TreeVizNode*) * 256);
                int q_front = 0, q_rear = -1;
                queue[++q_rear] = root;
                
                while (q_front <= q_rear && parent == NULL) {
                    TreeVizNode *current = queue[q_front++];
                    if (current && current->value == parent_val) {
                        parent = current;
                        break;
                    }
                    if (current) {
                        if (current->left) queue[++q_rear] = current->left;
                        if (current->right) queue[++q_rear] = current->right;
                    }
                }
                free(queue);
                
                if (parent) {
                    TreeVizNode *new_node = (TreeVizNode *)malloc(sizeof(TreeVizNode));
                    new_node->value = value;
                    new_node->parent_val = parent_val;
                    new_node->is_left = is_left;
                    new_node->left = NULL;
                    new_node->right = NULL;
                    
                    if (is_left) {
                        parent->left = new_node;
                    } else {
                        parent->right = new_node;
                    }
                    node_count++;
                }
            }
        }
        s = s->next;
    }
    
    if (root == NULL) {
        printf("\n  TREE: [EMPTY]\n\n");
        return;
    }
    
    printf("\n  BINARY SEARCH TREE\n\n");
    
    // Display tree showing only nodes inserted so far (incremental display)
    if (node_count == 0) {
        printf("  [EMPTY]\n\n");
        return;
    }
    
    // height not needed in incremental display here
    
    // Simple recursive display showing only existing nodes (no empty spaces)
    void printTreeHelper(TreeVizNode *node, const char *prefix, int is_last) {
        if (node == NULL) return;
        
        printf("%s", prefix);
        if (prefix[0] != '\0') {
            printf(is_last ? "+-- " : "|-- ");
        }
        printf("(%d)\n", node->value);
        
        // Build prefix for children
        char child_prefix[256];
        strcpy(child_prefix, prefix);
        if (prefix[0] != '\0') {
            strcat(child_prefix, is_last ? "    " : "|   ");
        } else {
            strcpy(child_prefix, "  ");
        }
        
        // Count existing children
        int has_children = 0;
        if (node->right) has_children++;
        if (node->left) has_children++;
        
        if (has_children == 0) return;
        
        // Print right child first, then left (for better visual)
        if (node->right) {
            printTreeHelper(node->right, child_prefix, node->left == NULL);
        }
        if (node->left) {
            printTreeHelper(node->left, child_prefix, 1);
        }
    }
    
    // Display tree incrementally (only shows nodes that exist)
    printTreeHelper(root, "", 1);
    
    // Free tree nodes
    TreeVizNode **nodes = (TreeVizNode **)malloc(sizeof(TreeVizNode*) * node_count);
    int node_idx = 0;
    TreeVizNode **free_queue = (TreeVizNode **)malloc(sizeof(TreeVizNode*) * 256);
    int fq_front = 0, fq_rear = -1;
    
    free_queue[++fq_rear] = root;
    while (fq_front <= fq_rear) {
        TreeVizNode *current = free_queue[fq_front++];
        if (current) {
            nodes[node_idx++] = current;
            if (current->left) free_queue[++fq_rear] = current->left;
            if (current->right) free_queue[++fq_rear] = current->right;
        }
    }
    
    for (int i = 0; i < node_idx; i++) {
        free(nodes[i]);
    }
    free(nodes);
    free(free_queue);
    printf("\n");
}

// Display graph structure
static void displayGraph(Step *steps) {
    int nodes[256];
    int node_count = 0;
    int edges[256][2];
    int edge_count = 0;
    int visited_nodes[256] = {0};
    
    // Collect nodes and edges from steps
    Step *step = steps;
    while (step) {
        if (strcmp(step->action, "add_node") == 0 && step->index_count >= 1) {
            int node = step->indices[0];
            if (!visited_nodes[node]) {
                nodes[node_count++] = node;
                visited_nodes[node] = 1;
            }
        } else if (strcmp(step->action, "add_edge") == 0 && step->index_count >= 2) {
            edges[edge_count][0] = step->indices[0];
            edges[edge_count][1] = step->indices[1];
            edge_count++;
        }
        step = step->next;
    }
    
    printf("\n  GRAPH\n");
    printf("  Nodes: ");
    for (int i = 0; i < node_count; i++) {
        printf("[%d] ", nodes[i]);
    }
    printf("\n\n");
    printf("  Edges:\n");
    for (int i = 0; i < edge_count; i++) {
        printf("    %d ---- %d\n", edges[i][0], edges[i][1]);
    }
    printf("\n");
}

// Get algorithm-specific description
static void printAlgorithmStep(JSONData *data, Step *step, int *working_array) {
    const char *alg = data->algorithm;
    
    if (strncmp(alg, "Bubble", 6) == 0) {
        // Bubble Sort specific descriptions
        if (strcmp(step->action, "compare") == 0 && step->index_count >= 2) {
            int i = step->indices[0];
            int j = step->indices[1];
            current_comparison++;
            printf("\n[Step %d] BUBBLE SORT - Comparing adjacent elements\n", current_comparison);
            printf("  -> Position %d (%d) vs Position %d (%d)\n", i, working_array[i], j, working_array[j]);
            printf("  -> %s\n", working_array[i] > working_array[j] ? 
                   "Out of order: Swap needed!" : "In order: No swap needed");
        } else if (strcmp(step->action, "swap") == 0 && step->index_count >= 2) {
            int i = step->indices[0];
            int j = step->indices[1];
            total_swaps++;
            printf("\n[Swap #%d] BUBBLE SORT - Swapping elements\n", total_swaps);
            printf("  -> Swapping %d (pos %d) and %d (pos %d)\n", 
                   working_array[j], i, working_array[i], j);
            printf("  -> Reason: %d > %d (larger element moves right)\n", 
                   working_array[i], working_array[j]);
        } else if (strcmp(step->action, "mark_sorted") == 0 && step->index_count >= 1) {
            current_pass++;
            int idx = step->indices[0];
            printf("\n[Pass %d Complete] BUBBLE SORT\n", current_pass);
            printf("  -> Element at position %d (%d) is now in final sorted position\n", 
                   idx, working_array[idx]);
            printf("  -> Largest unsorted element has 'bubbled' to the right\n");
        }
    } else if (strncmp(alg, "Insertion", 9) == 0) {
        // Insertion Sort specific descriptions
        if (strcmp(step->action, "compare") == 0 && step->index_count >= 2) {
            int i = step->indices[0];
            int j = step->indices[1];
            current_comparison++;
            printf("\n[Step %d] INSERTION SORT - Comparing key with sorted portion\n", current_comparison);
            printf("  -> Key element: %d (position %d)\n", working_array[j], j);
            printf("  -> Comparing with: %d (position %d)\n", working_array[i], i);
            printf("  -> %s\n", working_array[i] > working_array[j] ? 
                   "Key is smaller: Need to shift" : "Key is larger: Insert here");
        } else if (strcmp(step->action, "swap") == 0 && step->index_count >= 2) {
            int i = step->indices[0];
            int j = step->indices[1];
            total_swaps++;
            printf("\n[Shift #%d] INSERTION SORT - Shifting elements\n", total_swaps);
            printf("  -> Shifting %d from position %d to position %d\n", 
                   working_array[i], i, j);
        } else if (strcmp(step->action, "mark_sorted") == 0 && step->index_count >= 1) {
            int idx = step->indices[0];
            printf("\n[Insertion Complete] INSERTION SORT\n");
            printf("  -> Element at position %d (%d) is now in sorted position\n", 
                   idx, working_array[idx]);
            printf("  -> Left portion [0..%d] is now sorted\n", idx);
        }
    } else if (strncmp(alg, "Selection", 9) == 0) {
        // Selection Sort specific descriptions
        if (strcmp(step->action, "compare") == 0 && step->index_count >= 2) {
            int i = step->indices[0];
            int j = step->indices[1];
            current_comparison++;
            printf("\n[Step %d] SELECTION SORT - Finding minimum\n", current_comparison);
            printf("  -> Current minimum: %d (position %d)\n", working_array[i], i);
            printf("  -> Comparing with: %d (position %d)\n", working_array[j], j);
            printf("  -> %s\n", working_array[j] < working_array[i] ? 
                   "New minimum found!" : "Current minimum is smaller");
        } else if (strcmp(step->action, "swap") == 0 && step->index_count >= 2) {
            int i = step->indices[0];
            int j = step->indices[1];
            total_swaps++;
            printf("\n[Swap #%d] SELECTION SORT - Placing minimum\n", total_swaps);
            printf("  -> Swapping minimum %d (pos %d) with element at position %d\n", 
                   working_array[j], j, i);
            printf("  -> Minimum element placed in its correct position\n");
        } else if (strcmp(step->action, "mark_sorted") == 0 && step->index_count >= 1) {
            current_pass++;
            int idx = step->indices[0];
            printf("\n[Pass %d Complete] SELECTION SORT\n", current_pass);
            printf("  -> Position %d (%d) now contains the minimum element\n", 
                   idx, working_array[idx]);
            printf("  -> Elements [0..%d] are in final sorted positions\n", idx);
        }
    } else if (strncmp(alg, "Merge", 5) == 0) {
        // Merge Sort specific descriptions
        if (strcmp(step->action, "compare") == 0 && step->index_count >= 2) {
            int i = step->indices[0];
            int j = step->indices[1];
            current_comparison++;
            printf("\n[Step %d] MERGE SORT - Merging two sorted subarrays\n", current_comparison);
            printf("  -> Left subarray element: %d (position %d)\n", working_array[i], i);
            printf("  -> Right subarray element: %d (position %d)\n", working_array[j], j);
            printf("  -> %s\n", working_array[i] <= working_array[j] ? 
                   "Taking from left subarray" : "Taking from right subarray");
        } else if (strcmp(step->action, "swap") == 0 && step->index_count >= 2) {
            printf("\n[Merge Step] MERGE SORT - Merging elements\n");
            printf("  -> Combining elements from two sorted halves\n");
        } else if (strcmp(step->action, "mark_sorted") == 0 && step->index_count >= 1) {
            int idx = step->indices[0];
            printf("\n[Merge Complete] MERGE SORT\n");
            printf("  -> Subarray containing position %d is now sorted\n", idx);
        }
    } else if (strncmp(alg, "Quick", 5) == 0) {
        // Quick Sort specific descriptions
        if (strcmp(step->action, "compare") == 0 && step->index_count >= 2) {
            int i = step->indices[0];
            int j = step->indices[1];
            current_comparison++;
            printf("\n[Step %d] QUICK SORT - Partitioning\n", current_comparison);
            printf("  -> Pivot element: %d (position %d)\n", working_array[j], j);
            printf("  -> Comparing with: %d (position %d)\n", working_array[i], i);
            printf("  -> %s\n", working_array[i] <= working_array[j] ? 
                   "Element <= pivot: Move to left" : "Element > pivot: Move to right");
        } else if (strcmp(step->action, "swap") == 0 && step->index_count >= 2) {
            int i = step->indices[0];
            int j = step->indices[1];
            total_swaps++;
            printf("\n[Swap #%d] QUICK SORT - Partitioning swap\n", total_swaps);
            printf("  -> Swapping elements at positions %d and %d\n", i, j);
            printf("  -> Positioning elements relative to pivot\n");
        } else if (strcmp(step->action, "mark_sorted") == 0 && step->index_count >= 1) {
            int idx = step->indices[0];
            printf("\n[Partition Complete] QUICK SORT\n");
            printf("  -> Pivot at position %d (%d) is in final position\n", 
                   idx, working_array[idx]);
            printf("  -> Elements left of pivot are smaller, right are larger\n");
        }
    }
}

// Visualize step
static void visualizeStep(JSONData *data, Step *step, int *working_array) {
    if (should_exit) return;
    
    CLEAR_SCREEN();
    printf("=============================================================\n");
    printf("  %s Visualization\n", data->algorithm);
    printf("=============================================================\n");
    
    // Show appropriate metrics based on algorithm type
    const char *alg = data->algorithm;
    int is_stack = (strncmp(alg, "Stack", 5) == 0);
    int is_queue = (strncmp(alg, "Queue", 5) == 0);
    int is_tree = (strstr(alg, "Tree") != NULL);
    int is_graph = (strncmp(alg, "Graph", 5) == 0);
    int is_sorting = !is_stack && !is_queue && !is_tree && !is_graph;
    
    if (is_sorting) {
        printf("  Comparisons: %d  |  Swaps: %d  |  Pass: %d\n", 
               current_comparison, total_swaps, current_pass);
    } else if (is_stack) {
        printf("  Push: %d  |  Pop: %d  |  Total: %d\n", 
               stack_pushes, stack_pops, stack_pushes + stack_pops);
    } else if (is_queue) {
        printf("  Enqueue: %d  |  Dequeue: %d  |  Total: %d\n", 
               queue_enqueues, queue_dequeues, queue_enqueues + queue_dequeues);
    } else if (is_tree) {
        printf("  Inserts: %d  |  Visits: %d  |  Total: %d\n", 
               tree_inserts, tree_visits, tree_inserts + tree_visits);
    } else if (is_graph) {
        printf("  Nodes: %d  |  Edges: %d  |  Total: %d\n", 
               graph_nodes, graph_edges, graph_nodes + graph_edges);
    }
    printf("=============================================================\n\n");
    
    if (strcmp(step->action, "compare") == 0 && step->index_count >= 2) {
        int i = step->indices[0];
        int j = step->indices[1];
        
        printAlgorithmStep(data, step, working_array);
        displayArray(working_array, data->array_size, i, j, -1);
        
    } else if (strcmp(step->action, "swap") == 0 && step->index_count >= 2) {
        int i = step->indices[0];
        int j = step->indices[1];
        
        // Perform swap in working array
        int temp = working_array[i];
        working_array[i] = working_array[j];
        working_array[j] = temp;
        
        printAlgorithmStep(data, step, working_array);
        displayArray(working_array, data->array_size, i, j, -1);
        
    } else if (strcmp(step->action, "no_swap") == 0 && step->index_count >= 2) {
        int i = step->indices[0];
        int j = step->indices[1];
        
        printAlgorithmStep(data, step, working_array);
        printf("  -> Elements are already in correct order\n");
        displayArray(working_array, data->array_size, i, j, -1);
        
    } else if (strcmp(step->action, "mark_sorted") == 0 && step->index_count >= 1) {
        int idx = step->indices[0];
        
        printAlgorithmStep(data, step, working_array);
        displayArray(working_array, data->array_size, -1, -1, idx + 1);
        
    } else if (strcmp(step->action, "push") == 0 && step->index_count >= 2) {
        int pos = step->indices[0];
        int val = step->indices[1];
        stack_pushes++;
        printf("\n[PUSH] Stack Operation\n");
        printf("  -> Pushing %d onto stack (position %d)\n", val, pos);
        if (pos >= 0 && pos < data->array_size) {
            working_array[pos] = val;
        }
        // Display stack
        int stack_arr[256] = {0};
        int stack_top = -1;
        // Build stack from all push/pop operations so far
        Step *s = data->steps;
        while (s && s != step->next) {
            if (strcmp(s->action, "push") == 0 && s->index_count >= 2) {
                stack_top++;
                stack_arr[stack_top] = s->indices[1];
            } else if (strcmp(s->action, "pop") == 0 && s->index_count >= 2) {
                if (stack_top >= 0) stack_top--;
            }
            s = s->next;
        }
        displayStack(stack_arr, stack_top, stack_top);
        
    } else if (strcmp(step->action, "pop") == 0 && step->index_count >= 2) {
        int pos = step->indices[0];
        int val = step->indices[1];
        stack_pops++;
        printf("\n[POP] Stack Operation\n");
        printf("  -> Popping %d from stack (was at position %d)\n", val, pos);
        // Display stack
        int stack_arr[256] = {0};
        int stack_top = -1;
        Step *s = data->steps;
        while (s && s != step) {
            if (strcmp(s->action, "push") == 0 && s->index_count >= 2) {
                stack_top++;
                stack_arr[stack_top] = s->indices[1];
            } else if (strcmp(s->action, "pop") == 0 && s->index_count >= 2) {
                if (stack_top >= 0) stack_top--;
            }
            s = s->next;
        }
        displayStack(stack_arr, stack_top, -1);
        
    } else if (strcmp(step->action, "enqueue") == 0 && step->index_count >= 2) {
        int val = step->indices[1];
        queue_enqueues++;
        printf("\n[ENQUEUE] Queue Operation\n");
        printf("  -> Adding %d to rear of queue\n", val);
        // Display queue
        int queue_arr[256] = {0};
        int front = 0, rear = -1;
        Step *s = data->steps;
        while (s && s != step->next) {
            if (strcmp(s->action, "enqueue") == 0 && s->index_count >= 2) {
                rear++;
                queue_arr[rear] = s->indices[1];
            } else if (strcmp(s->action, "dequeue") == 0 && s->index_count >= 2) {
                if (front <= rear) front++;
            }
            s = s->next;
        }
        displayQueue(queue_arr, front, rear, rear);
        
    } else if (strcmp(step->action, "dequeue") == 0 && step->index_count >= 2) {
        int val = step->indices[1];
        queue_dequeues++;
        printf("\n[DEQUEUE] Queue Operation\n");
        printf("  -> Removing %d from front of queue\n", val);
        // Display queue
        int queue_arr[256] = {0};
        int front = 0, rear = -1;
        Step *s = data->steps;
        while (s && s != step) {
            if (strcmp(s->action, "enqueue") == 0 && s->index_count >= 2) {
                rear++;
                queue_arr[rear] = s->indices[1];
            } else if (strcmp(s->action, "dequeue") == 0 && s->index_count >= 2) {
                if (front <= rear) front++;
            }
            s = s->next;
        }
        displayQueue(queue_arr, front, rear, front - 1);
        
    } else if (strcmp(step->action, "insert") == 0 && step->index_count >= 4) {
        int value = step->indices[0];
        int parent_val = step->indices[1];
        int is_left = step->indices[2];
        tree_inserts++;
        printf("\n[INSERT] Tree Operation\n");
        if (parent_val == -1) {
            printf("  -> Inserting %d as root node\n", value);
        } else {
            printf("  -> Inserting %d as %s child of %d\n", 
                   value, is_left ? "left" : "right", parent_val);
        }
        // Display tree up to this point (including current step)
        displayTree(data->steps, step);
        
    } else if (strcmp(step->action, "visit") == 0 && step->index_count >= 1) {
        int value = step->indices[0];
        tree_visits++;
        printf("\n[VISIT] Tree Traversal\n");
        printf("  -> Visiting node with value %d\n", value);
        // For traversal, show the full tree state up to now
        Step *last_insert = data->steps;
        while (last_insert && last_insert->next) {
            if (strcmp(last_insert->action, "insert") == 0) {
                Step *temp = last_insert;
                while (temp && temp->next) temp = temp->next;
                if (temp && strcmp(temp->action, "insert") == 0) {
                    last_insert = temp;
                }
            }
            last_insert = last_insert->next;
        }
        displayTree(data->steps, step);
        
    } else if (strcmp(step->action, "add_node") == 0 && step->index_count >= 1) {
        int node = step->indices[0];
        graph_nodes++;
        printf("\n[ADD_NODE] Graph Operation\n");
        printf("  -> Adding node %d to graph\n", node);
        displayGraph(data->steps);
        
    } else if (strcmp(step->action, "add_edge") == 0 && step->index_count >= 2) {
        int from = step->indices[0];
        int to = step->indices[1];
        graph_edges++;
        printf("\n[ADD_EDGE] Graph Operation\n");
        printf("  -> Adding edge from node %d to node %d\n", from, to);
        displayGraph(data->steps);
        
    } else if (strcmp(step->action, "bfs_start") == 0 || strcmp(step->action, "dfs_start") == 0) {
        int start = step->indices[0];
        printf("\n[%s] Graph Traversal\n", 
               strcmp(step->action, "bfs_start") == 0 ? "BFS" : "DFS");
        printf("  -> Starting from node %d\n", start);
        displayGraph(data->steps);
        
    } else if (strcmp(step->action, "peek") == 0 && step->index_count >= 1) {
        int pos = step->indices[0];
        printf("\n[PEEK] Stack Operation\n");
        printf("  -> Top element is at position %d\n", pos);
        // Display stack
        int stack_arr[256] = {0};
        int stack_top = -1;
        Step *s = data->steps;
        while (s && s != step->next) {
            if (strcmp(s->action, "push") == 0 && s->index_count >= 2) {
                stack_top++;
                stack_arr[stack_top] = s->indices[1];
            } else if (strcmp(s->action, "pop") == 0 && s->index_count >= 2) {
                if (stack_top >= 0) stack_top--;
            }
            s = s->next;
        }
        displayStack(stack_arr, stack_top, stack_top);
        
    } else {
        // Generic action - use array display for sorting algorithms
        printf("-> %s\n", step->action);
        if (step->index_count > 0) {
            printf("  Indices: ");
            for (int i = 0; i < step->index_count; i++) {
                printf("%d ", step->indices[i]);
            }
            printf("\n");
        }
        // Only display array if it's a sorting algorithm
        if (strncmp(data->algorithm, "Stack", 5) != 0 && 
            strncmp(data->algorithm, "Queue", 5) != 0 &&
            strncmp(data->algorithm, "Binary", 6) != 0 &&
            strncmp(data->algorithm, "Graph", 5) != 0) {
            displayArray(working_array, data->array_size, -1, -1, -1);
        }
    }
    
    printf("\n[Press any key to continue... (or Ctrl+C to exit)]\n");
    
    // Check for exit signal before waiting for input
    if (should_exit) {
        return;
    }
    
    #ifdef _WIN32
    // On Windows, use _getch() which respects Ctrl+C
    // The console handler will set should_exit when Ctrl+C is pressed
    if (!should_exit) {
        _getch();
        // Check again after input
        if (should_exit) {
            return;
        }
    }
    #else
    // On Unix, getchar will be interrupted by signal
    if (!should_exit) {
        getchar();
        if (should_exit) {
            return;
        }
    }
    #endif
}

int main(int argc, char **argv) {
    const char *inputFile = (argc > 1) ? argv[1] : "output.json";
    
    #ifdef _WIN32
    // Enable UTF-8 on Windows (if supported)
    SetConsoleOutputCP(65001); // UTF-8
    
    // Set up Ctrl+C handler
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        fprintf(stderr, "Warning: Could not set console control handler\n");
    }
    #else
    // Set up signal handlers for Unix
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    #endif
    
    should_exit = 0;  // Reset exit flag
    
    // Reset algorithm tracking
    current_pass = 0;
    current_comparison = 0;
    total_swaps = 0;
    total_comparisons = 0;
    
    // Reset data structure tracking
    stack_pushes = 0;
    stack_pops = 0;
    queue_enqueues = 0;
    queue_dequeues = 0;
    tree_inserts = 0;
    tree_visits = 0;
    graph_nodes = 0;
    graph_edges = 0;
    
    printf("Loading visualization data from %s...\n\n", inputFile);
    
    JSONData *data = parseJSON(inputFile);
    if (!data) {
        fprintf(stderr, "Error: Failed to parse JSON file: %s\n", inputFile);
        return 1;
    }
    
    // Initialize working array
    int working_array[256];
    for (int i = 0; i < data->array_size; i++) {
        working_array[i] = data->array[i];
    }
    
    // Display initial state with algorithm overview
    CLEAR_SCREEN();
    printf("=============================================================\n");
    printf("  %s Visualization\n", data->algorithm);
    printf("=============================================================\n\n");
    
    // Display algorithm/data-structure overview
    const char *alg = data->algorithm;
    int is_stack = (strncmp(alg, "Stack", 5) == 0);
    int is_queue = (strncmp(alg, "Queue", 5) == 0);
    int is_tree  = (strstr(alg, "Tree") != NULL);
    int is_graph = (strncmp(alg, "Graph", 5) == 0);
    int is_sorting = !is_stack && !is_queue && !is_tree && !is_graph;

    if (is_sorting) {
        printf("Algorithm Overview:\n");
        if (strncmp(alg, "Bubble", 6) == 0) {
            printf("  -> Bubble Sort: Repeatedly compares adjacent elements and swaps if out of order\n");
            printf("  -> Each pass moves the largest unsorted element to its correct position\n");
            printf("  -> Continues until no more swaps are needed\n");
            printf("  -> Time Complexity: O(n²) | Space Complexity: O(1)\n");
        } else if (strncmp(alg, "Insertion", 9) == 0) {
            printf("  -> Insertion Sort: Builds sorted array one element at a time\n");
            printf("  -> Takes each element and inserts it into correct position in sorted portion\n");
            printf("  -> Efficient for small arrays or nearly sorted data\n");
            printf("  -> Time Complexity: O(n²) | Space Complexity: O(1)\n");
        } else if (strncmp(alg, "Selection", 9) == 0) {
            printf("  -> Selection Sort: Finds minimum element and places it at beginning\n");
            printf("  -> Divides array into sorted and unsorted portions\n");
            printf("  -> Repeatedly selects minimum from unsorted portion\n");
            printf("  -> Time Complexity: O(n²) | Space Complexity: O(1)\n");
        } else if (strncmp(alg, "Merge", 5) == 0) {
            printf("  -> Merge Sort: Divide and conquer algorithm\n");
            printf("  -> Divides array into halves, sorts each half, then merges them\n");
            printf("  -> Stable and has guaranteed O(n log n) time complexity\n");
            printf("  -> Time Complexity: O(n log n) | Space Complexity: O(n)\n");
        } else if (strncmp(alg, "Quick", 5) == 0) {
            printf("  -> Quick Sort: Divide and conquer with pivot-based partitioning\n");
            printf("  -> Selects pivot, partitions array around pivot, recursively sorts partitions\n");
            printf("  -> Average case O(n log n), efficient in practice\n");
            printf("  -> Time Complexity: O(n log n) avg, O(n²) worst | Space Complexity: O(log n)\n");
        }
        printf("\nInitial Array State:\n");
        displayArray(working_array, data->array_size, -1, -1, -1);
    } else if (is_queue) {
        printf("Queue Overview:\n");
        printf("  -> FIFO (First In First Out)\n");
        printf("  -> Time Complexities: Enqueue O(1), Dequeue O(1), Find O(n), Peek O(1)\n");
        printf("\nInitial Queue State:\n");
        int queue_arr[256] = {0};
        int front = 0, rear = -1; // empty
        displayQueue(queue_arr, front, rear, -1);
    } else if (is_stack) {
        printf("Stack Overview:\n");
        printf("  -> LIFO (Last In First Out)\n");
        printf("  -> Time Complexities: Push O(1), Pop O(1), Peek O(1), Search O(n)\n");
        printf("\nInitial Stack State:\n");
        int stack_arr[256] = {0};
        int stack_top = -1; // empty
        displayStack(stack_arr, stack_top, -1);
    } else if (is_tree) {
        printf("Binary Tree Overview:\n");
        printf("  -> Operations supported: Insert, Traversals (Inorder/Preorder/Postorder)\n");
        printf("  -> Time Complexities (BST avg): Insertion O(log n), Deletion O(log n), Search O(log n), Traversal O(n)\n");
        printf("\nInitial Tree Structure:\n");
        printf("  TREE: [EMPTY]\n\n");
    } else if (is_graph) {
        printf("Graph Overview:\n");
        printf("  -> Nodes and Edges; Traversals like BFS/DFS\n");
        printf("  -> Time Complexities: Add Node O(1), Add Edge O(1), BFS/DFS O(V+E)\n");
        printf("\nInitial Graph State:\n");
        printf("  [EMPTY]\n\n");
    }
    printf("\n[Press any key to start step-by-step visualization... (Ctrl+C to exit)]\n");
    
    #ifdef _WIN32
    int ch = _getch();
    if (ch == 3 || ch == 27) {  // Ctrl+C or ESC
        should_exit = 1;
    }
    #else
    int ch = getchar();
    #endif
    
    if (should_exit) {
        printf("\n[Exiting...]\n");
        // Cleanup
        Step *step = data->steps;
        while (step) {
            Step *next = step->next;
            free(step);
            step = next;
        }
        free(data);
        return 0;
    }
    
    // Visualize each step
    Step *step = data->steps;
    int step_num = 1;
    while (step && !should_exit) {
        visualizeStep(data, step, working_array);
        if (should_exit) break;
        step = step->next;
        step_num++;
    }
    
    // Final state (only if not interrupted)
    if (!should_exit) {
        CLEAR_SCREEN();
        const char *alg = data->algorithm;
        
        // Check if this is a data structure operation or sorting algorithm
        int is_stack = (strncmp(alg, "Stack", 5) == 0);
        int is_queue = (strncmp(alg, "Queue", 5) == 0);
        int is_tree = (strstr(alg, "Tree") != NULL);
        int is_graph = (strncmp(alg, "Graph", 5) == 0);
        int is_sorting = !is_stack && !is_queue && !is_tree && !is_graph;
        
        if (is_sorting) {
            // Sorting algorithm summary
            printf("=============================================================\n");
            printf("  Final Sorted Array - %s Complete\n", data->algorithm);
            printf("=============================================================\n");
            printf("  Total Comparisons: %d\n", current_comparison);
            printf("  Total Swaps: %d\n", total_swaps);
            printf("  Total Passes: %d\n", current_pass);
            printf("=============================================================\n\n");
            displayArray(working_array, data->array_size, -1, -1, data->array_size);
            printf("[SUCCESS] Algorithm completed successfully!\n");
            printf("\nSummary:\n");
            printf("  -> Array is now fully sorted\n");
            if (strncmp(alg, "Merge", 5) == 0 || strncmp(alg, "Quick", 5) == 0) {
                printf("  -> Time Complexity: O(n log n)\n");
            } else {
                printf("  -> Time Complexity: O(n²)\n");
            }
            printf("  -> Space Complexity: O(1) for in-place sorting\n");
        } else if (is_stack) {
            // Stack operations summary
            printf("=============================================================\n");
            printf("  Final Stack State — All Operations Complete\n");
            printf("=============================================================\n");
            printf("  Total Push Operations: %d\n", stack_pushes);
            printf("  Total Pop Operations: %d\n", stack_pops);
            printf("  Total Operations: %d\n", stack_pushes + stack_pops);
            printf("=============================================================\n\n");
            // Display final stack state
            int stack_arr[256] = {0};
            int stack_top = -1;
            Step *s = data->steps;
            while (s) {
                if (strcmp(s->action, "push") == 0 && s->index_count >= 2) {
                    stack_top++;
                    stack_arr[stack_top] = s->indices[1];
                } else if (strcmp(s->action, "pop") == 0 && s->index_count >= 2) {
                    if (stack_top >= 0) stack_top--;
                }
                s = s->next;
            }
            displayStack(stack_arr, stack_top, -1);
            printf("[SUCCESS] Stack operations completed successfully!\n");
            printf("\nSummary:\n");
            printf("  -> Stack follows LIFO (Last In First Out) principle\n");
            printf("  -> Push Time Complexity: O(1)\n");
            printf("  -> Pop Time Complexity: O(1)\n");
            printf("  -> Peek Time Complexity: O(1)\n");
            printf("  -> Space Complexity: O(n) where n is number of elements\n");
            if (stack_top >= 0) {
                printf("  -> Current stack size: %d element(s)\n", stack_top + 1);
                printf("  -> Top element: %d\n", stack_arr[stack_top]);
            } else {
                printf("  -> Stack is currently empty\n");
            }
        } else if (is_queue) {
            // Queue operations summary
            printf("=============================================================\n");
            printf("  Final Queue State — All Operations Complete\n");
            printf("=============================================================\n");
            printf("  Total Enqueue Operations: %d\n", queue_enqueues);
            printf("  Total Dequeue Operations: %d\n", queue_dequeues);
            printf("  Total Operations: %d\n", queue_enqueues + queue_dequeues);
            printf("=============================================================\n\n");
            // Display final queue state
            int queue_arr[256] = {0};
            int front = 0, rear = -1;
            Step *s = data->steps;
            while (s) {
                if (strcmp(s->action, "enqueue") == 0 && s->index_count >= 2) {
                    rear++;
                    queue_arr[rear] = s->indices[1];
                } else if (strcmp(s->action, "dequeue") == 0 && s->index_count >= 2) {
                    if (front <= rear) front++;
                }
                s = s->next;
            }
            displayQueue(queue_arr, front, rear, -1);
            printf("[SUCCESS] Queue operations completed successfully!\n");
            printf("\nSummary:\n");
            printf("  -> Queue follows FIFO (First In First Out) principle\n");
            printf("  -> Enqueue Time Complexity: O(1)\n");
            printf("  -> Dequeue Time Complexity: O(1)\n");
            printf("  -> Space Complexity: O(n) where n is number of elements\n");
            int queue_size = (rear >= front) ? (rear - front + 1) : 0;
            if (queue_size > 0) {
                printf("  -> Current queue size: %d element(s)\n", queue_size);
                printf("  -> Front element: %d\n", queue_arr[front]);
                printf("  -> Rear element: %d\n", queue_arr[rear]);
            } else {
                printf("  -> Queue is currently empty\n");
            }
        } else if (is_tree) {
            // Tree operations summary
            printf("=============================================================\n");
            printf("  Final Tree Structure — All Operations Complete\n");
            printf("=============================================================\n");
            printf("  Total Insert Operations: %d\n", tree_inserts);
            printf("  Total Visit Operations: %d\n", tree_visits);
            printf("  Total Operations: %d\n", tree_inserts + tree_visits);
            printf("=============================================================\n\n");
            // Display final tree state and compute metadata
            int node_count_meta = 0;
            TreeVizNode *root_meta = buildTreeFromSteps(data->steps, &node_count_meta);
            displayTree(data->steps, NULL);
            printf("[SUCCESS] Tree operations completed successfully!\n");
            printf("\nSummary:\n");
            printf("  -> Binary Search Tree (BST) structure\n");
            printf("  -> Insert Time Complexity: O(log n) average, O(n) worst case\n");
            printf("  -> Search Time Complexity: O(log n) average, O(n) worst case\n");
            printf("  -> Traversal Time Complexity: O(n) - visits all nodes\n");
            printf("  -> Space Complexity: O(n) where n is number of nodes\n");
            if (node_count_meta > 0 && root_meta) {
                int height_meta = treeHeight(root_meta);
                printf("  -> Total nodes in tree: %d\n", node_count_meta);
                printf("  -> Root node value: %d\n", root_meta->value);
                printf("  -> Height of tree: %d\n", height_meta);
            } else {
                printf("  -> Tree is currently empty\n");
            }
        } else if (is_graph) {
            // Graph operations summary
            printf("=============================================================\n");
            printf("  Final Graph State — All Operations Complete\n");
            printf("=============================================================\n");
            printf("  Total Nodes Added: %d\n", graph_nodes);
            printf("  Total Edges Added: %d\n", graph_edges);
            printf("  Total Operations: %d\n", graph_nodes + graph_edges);
            printf("=============================================================\n\n");
            // Display final graph state
            displayGraph(data->steps);
            printf("[SUCCESS] Graph operations completed successfully!\n");
            printf("\nSummary:\n");
            printf("  -> Graph data structure (undirected)\n");
            printf("  -> Add Node Time Complexity: O(1)\n");
            printf("  -> Add Edge Time Complexity: O(1)\n");
            printf("  -> BFS/DFS Time Complexity: O(V + E) where V=vertices, E=edges\n");
            printf("  -> Space Complexity: O(V + E) for adjacency representation\n");
            if (graph_nodes > 0) {
                printf("  -> Total vertices: %d\n", graph_nodes);
                printf("  -> Total edges: %d\n", graph_edges);
            } else {
                printf("  -> Graph is currently empty\n");
            }
        }
    }
    
    // Cleanup
    step = data->steps;
    while (step) {
        Step *next = step->next;
        free(step);
        step = next;
    }
    free(data);
    
    return 0;
}

