// algorithms.c
// Implementations of sorting algorithms and basic data-structure operations
// Designed to be called by a compiler backend (e.g., codegen.c) which may
// pass a callback to collect IR/visualization steps.
//
// The file provides:
//  - Sorting: bubble_sort, selection_sort, insertion_sort, quick_sort, merge_sort
//  - Stack: stack_create, stack_push, stack_pop, stack_peek
//  - Queue: queue_create, queue_enqueue, queue_dequeue, queue_peek
//  - Binary Search Tree (BST): bst_insert, bst_traverse_inorder/preorder/postorder
//  - Graph: graph_create, graph_add_node, graph_add_edge, graph_bfs, graph_dfs
//
// Each algorithm accepts an optional callback of signature `ir_callback_t`
// which is invoked with descriptive actions and integer arguments so the
// compiler/visualizer can build the IR or JSON steps.
//
// Example callback signature:
//    typedef void (*ir_callback_t)(const char *action, const int *args, int nargs, void *ctx);
// `action`   - a short string like "compare", "swap", "push", "enqueue", "insert", "visit"
// `args`     - integer arguments (indices, values, node ids) meaningful per action
// `nargs`    - number of args in the args array
// `ctx`      - user-provided context passed through (e.g., pointer to IR accumulator)
//
// If callback is NULL, the algorithm runs normally but does not emit steps.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Callback type used to emit IR steps to the rest of the pipeline */
typedef void (*ir_callback_t)(const char *action, const int *args, int nargs, void *ctx);

/* ---------------------------
   Utility helpers
   --------------------------- */
static void emit(ir_callback_t cb, const char *action, const int *args, int nargs, void *ctx) {
    if (cb) cb(action, args, nargs, ctx);
}

static void swap_int(int *a, int *b) {
    int t = *a; *a = *b; *b = t;
}

/* ---------------------------
   Sorting algorithms
   Each sorting function receives:
     - arr: pointer to int array
     - n: length of array
     - cb: optional callback (can be NULL)
     - ctx: optional context passed to callback
   Callbacks emit actions suitable for visualization/IR.
   --------------------------- */

/* Bubble Sort */
void bubble_sort(int *arr, int n, ir_callback_t cb, void *ctx) {
    int comparisons = 0, swaps = 0;
    for (int pass = 0; pass < n - 1; ++pass) {
        for (int i = 0; i < n - pass - 1; ++i) {
            int args_cmp[2] = { i, i+1 };
            emit(cb, "compare", args_cmp, 2, ctx);  // compare i and i+1
            ++comparisons;
            if (arr[i] > arr[i+1]) {
                int args_swap[2] = { i, i+1 };
                swap_int(&arr[i], &arr[i+1]);
                emit(cb, "swap", args_swap, 2, ctx); // swap indices
                ++swaps;
            }
        }
        int args_mark[1] = { n - pass - 1 };
        emit(cb, "mark_sorted", args_mark, 1, ctx); // mark last element as sorted in this pass
    }
    int summary[2] = { comparisons, swaps };
    emit(cb, "summary_sort", summary, 2, ctx);
}

/* Selection Sort */
void selection_sort(int *arr, int n, ir_callback_t cb, void *ctx) {
    int comparisons = 0, swaps = 0;
    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n; ++j) {
            int args_cmp[2] = { min_idx, j };
            emit(cb, "compare", args_cmp, 2, ctx);
            ++comparisons;
            if (arr[j] < arr[min_idx]) min_idx = j;
        }
        if (min_idx != i) {
            int args_swap[2] = { i, min_idx };
            swap_int(&arr[i], &arr[min_idx]);
            emit(cb, "swap", args_swap, 2, ctx);
            ++swaps;
        }
        int args_mark[1] = { i };
        emit(cb, "mark_sorted", args_mark, 1, ctx);
    }
    int summary[2] = { comparisons, swaps };
    emit(cb, "summary_sort", summary, 2, ctx);
}

/* Insertion Sort */
void insertion_sort(int *arr, int n, ir_callback_t cb, void *ctx) {
    int comparisons = 0, shifts = 0;
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        // Compare and shift
        while (j >= 0) {
            int args_cmp[2] = { j, i };
            emit(cb, "compare", args_cmp, 2, ctx);
            ++comparisons;
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                int args_shift[2] = { j, j+1 }; // from j to j+1
                emit(cb, "shift", args_shift, 2, ctx);
                ++shifts;
                --j;
            } else break;
        }
        arr[j + 1] = key;
        int args_insert[2] = { j+1, key };
        emit(cb, "insert", args_insert, 2, ctx); // index and value
    }
    int summary[2] = { comparisons, shifts };
    emit(cb, "summary_sort", summary, 2, ctx);
}

/* Quick Sort helpers with callbacks */
static int partition_with_cb(int *arr, int low, int high, ir_callback_t cb, void *ctx) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        int args_cmp[2] = { j, high }; // compare arr[j] with pivot at high
        emit(cb, "compare_pivot", args_cmp, 2, ctx);
        if (arr[j] <= pivot) {
            ++i;
            if (i != j) {
                swap_int(&arr[i], &arr[j]);
                int args_swap[2] = { i, j };
                emit(cb, "swap", args_swap, 2, ctx);
            }
        }
    }
    if (i+1 != high) {
        swap_int(&arr[i+1], &arr[high]);
        int args_swap[2] = { i+1, high };
        emit(cb, "swap", args_swap, 2, ctx);
    }
    int pivot_idx[1] = { i+1 };
    emit(cb, "pivot_placed", pivot_idx, 1, ctx);
    return i + 1;
}

static void quick_sort_rec(int *arr, int low, int high, ir_callback_t cb, void *ctx) {
    if (low < high) {
        int pi = partition_with_cb(arr, low, high, cb, ctx);
        quick_sort_rec(arr, low, pi - 1, cb, ctx);
        quick_sort_rec(arr, pi + 1, high, cb, ctx);
    } else if (low == high) {
        int args_mark[1] = { low };
        emit(cb, "mark_sorted", args_mark, 1, ctx);
    }
}

void quick_sort(int *arr, int n, ir_callback_t cb, void *ctx) {
    quick_sort_rec(arr, 0, n - 1, cb, ctx);
    emit(cb, "summary_sort_done", NULL, 0, ctx);
}

/* Merge Sort helpers with callbacks */
static void merge_with_cb(int *arr, int l, int m, int r, ir_callback_t cb, void *ctx) {
    int n1 = m - l + 1;
    int n2 = r - m;
    int *L = (int*)malloc(sizeof(int) * n1);
    int *R = (int*)malloc(sizeof(int) * n2);
    for (int i = 0; i < n1; ++i) L[i] = arr[l + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[m + 1 + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        int args_cmp[2] = { l + i, m + 1 + j };
        emit(cb, "compare", args_cmp, 2, ctx);
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    free(L); free(R);
    int args_merged[2] = { l, r };
    emit(cb, "merged_segment", args_merged, 2, ctx);
}

static void merge_sort_rec(int *arr, int l, int r, ir_callback_t cb, void *ctx) {
    if (l < r) {
        int m = (l + r) / 2;
        merge_sort_rec(arr, l, m, cb, ctx);
        merge_sort_rec(arr, m + 1, r, cb, ctx);
        merge_with_cb(arr, l, m, r, cb, ctx);
    } else if (l == r) {
        int args_mark[1] = { l };
        emit(cb, "mark_sorted", args_mark, 1, ctx);
    }
}

void merge_sort(int *arr, int n, ir_callback_t cb, void *ctx) {
    merge_sort_rec(arr, 0, n - 1, cb, ctx);
    emit(cb, "summary_sort_done", NULL, 0, ctx);
}

/* ---------------------------
   Stack (LIFO) implementation
   --------------------------- */

typedef struct {
    int *data;
    int capacity;
    int top; // index of top element (-1 if empty)
    ir_callback_t cb;
    void *ctx;
} Stack;

Stack *stack_create(int capacity, ir_callback_t cb, void *ctx) {
    Stack *s = (Stack*)malloc(sizeof(Stack));
    s->data = (int*)malloc(sizeof(int) * capacity);
    s->capacity = capacity;
    s->top = -1;
    s->cb = cb;
    s->ctx = ctx;
    emit(cb, "stack_init", NULL, 0, ctx);
    return s;
}

void stack_push(Stack *s, int value) {
    if (s->top + 1 >= s->capacity) {
        // simple resize
        int newcap = s->capacity * 2;
        s->data = (int*)realloc(s->data, sizeof(int) * newcap);
        s->capacity = newcap;
    }
    s->data[++s->top] = value;
    int args[2] = { s->top, value };
    emit(s->cb, "push", args, 2, s->ctx);
}

int stack_pop(Stack *s, int *out_value) {
    if (s->top < 0) {
        emit(s->cb, "pop_empty", NULL, 0, s->ctx);
        return 0; // empty
    }
    int v = s->data[s->top--];
    if (out_value) *out_value = v;
    int args[1] = { v };
    emit(s->cb, "pop", args, 1, s->ctx);
    return 1;
}

int stack_peek(Stack *s, int *out_value) {
    if (s->top < 0) return 0;
    if (out_value) *out_value = s->data[s->top];
    int args[2] = { s->top, s->data[s->top] };
    emit(s->cb, "peek", args, 2, s->ctx);
    return 1;
}

void stack_free(Stack *s) {
    if (!s) return;
    free(s->data);
    emit(s->cb, "stack_free", NULL, 0, s->ctx);
    free(s);
}

/* ---------------------------
   Queue (FIFO) implementation - circular buffer
   --------------------------- */

typedef struct {
    int *data;
    int capacity;
    int front; // index of front element
    int size;  // number of elements
    ir_callback_t cb;
    void *ctx;
} Queue;

Queue *queue_create(int capacity, ir_callback_t cb, void *ctx) {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    q->data = (int*)malloc(sizeof(int) * capacity);
    q->capacity = capacity;
    q->front = 0;
    q->size = 0;
    q->cb = cb;
    q->ctx = ctx;
    emit(cb, "queue_init", NULL, 0, ctx);
    return q;
}

void queue_enqueue(Queue *q, int value) {
    if (q->size >= q->capacity) {
        // resize: simple reallocation with re-indexing
        int newcap = q->capacity * 2;
        int *newdata = (int*)malloc(sizeof(int) * newcap);
        for (int i = 0; i < q->size; ++i)
            newdata[i] = q->data[(q->front + i) % q->capacity];
        free(q->data);
        q->data = newdata;
        q->capacity = newcap;
        q->front = 0;
    }
    int idx = (q->front + q->size) % q->capacity;
    q->data[idx] = value;
    ++q->size;
    int args[2] = { idx, value };
    emit(q->cb, "enqueue", args, 2, q->ctx);
}

int queue_dequeue(Queue *q, int *out_value) {
    if (q->size == 0) {
        emit(q->cb, "dequeue_empty", NULL, 0, q->ctx);
        return 0;
    }
    int v = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    --q->size;
    if (out_value) *out_value = v;
    int args[1] = { v };
    emit(q->cb, "dequeue", args, 1, q->ctx);
    return 1;
}

int queue_peek(Queue *q, int *out_value) {
    if (q->size == 0) return 0;
    if (out_value) *out_value = q->data[q->front];
    int args[2] = { q->front, q->data[q->front] };
    emit(q->cb, "queue_peek", args, 2, q->ctx);
    return 1;
}

void queue_free(Queue *q) {
    if (!q) return;
    free(q->data);
    emit(q->cb, "queue_free", NULL, 0, q->ctx);
    free(q);
}

/* ---------------------------
   Binary Search Tree (BST)
   --------------------------- */

typedef struct BSTNode {
    int value;
    struct BSTNode *left, *right;
} BSTNode;

BSTNode* bst_new_node(int value) {
    BSTNode *n = (BSTNode*)malloc(sizeof(BSTNode));
    n->value = value;
    n->left = n->right = NULL;
    return n;
}


BSTNode* bst_insert_with_cb(BSTNode *root, int value, ir_callback_t cb, void *ctx) {
    if (root == NULL) {
        BSTNode *node = bst_new_node(value);
        int args[1] = { value };
        emit(cb, "bst_inserted", args, 1, ctx);
        return node;
    }
    int args_cmp[2] = { root->value, value };
    emit(cb, "bst_compare", args_cmp, 2, ctx);
    if (value < root->value) {
        root->left = bst_insert_with_cb(root->left, value, cb, ctx);
    } else {
        root->right = bst_insert_with_cb(root->right, value, cb, ctx);
    }
    return root;
}

void bst_inorder_with_cb(BSTNode *root, ir_callback_t cb, void *ctx) {
    if (!root) return;
    bst_inorder_with_cb(root->left, cb, ctx);
    int args[1] = { root->value };
    emit(cb, "bst_visit", args, 1, ctx);
    bst_inorder_with_cb(root->right, cb, ctx);
}

void bst_preorder_with_cb(BSTNode *root, ir_callback_t cb, void *ctx) {
    if (!root) return;
    int args[1] = { root->value };
    emit(cb, "bst_visit", args, 1, ctx);
    bst_preorder_with_cb(root->left, cb, ctx);
    bst_preorder_with_cb(root->right, cb, ctx);
}

void bst_postorder_with_cb(BSTNode *root, ir_callback_t cb, void *ctx) {
    if (!root) return;
    bst_postorder_with_cb(root->left, cb, ctx);
    bst_postorder_with_cb(root->right, cb, ctx);
    int args[1] = { root->value };
    emit(cb, "bst_visit", args, 1, ctx);
}

void bst_free(BSTNode *root) {
    if (!root) return;
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}


typedef struct AdjNode {
    int v;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    int max_nodes;
    AdjNode **adj; // array of adjacency lists
    int *present;  // 1 if node exists, else 0
    ir_callback_t cb;
    void *ctx;
} Graph;


Graph *graph_create(int max_nodes, ir_callback_t cb, void *ctx) {
    Graph *g = (Graph*)malloc(sizeof(Graph));
    g->max_nodes = max_nodes;
    g->adj = (AdjNode**)malloc(sizeof(AdjNode*) * max_nodes);
    g->present = (int*)malloc(sizeof(int) * max_nodes);
    for (int i = 0; i < max_nodes; ++i) { g->adj[i] = NULL; g->present[i] = 0; }
    g->cb = cb; g->ctx = ctx;
    emit(cb, "graph_init", NULL, 0, ctx);
    return g;
}

void graph_add_node(Graph *g, int id) {
    if (id < 0 || id >= g->max_nodes) return;
    if (!g->present[id]) {
        g->present[id] = 1;
        int args[1] = { id };
        emit(g->cb, "add_node", args, 1, g->ctx);
    }
}

void graph_add_edge(Graph *g, int u, int v) {
    if (u < 0 || u >= g->max_nodes || v < 0 || v >= g->max_nodes) return;
    // add u->v
    AdjNode *n1 = (AdjNode*)malloc(sizeof(AdjNode));
    n1->v = v; n1->next = g->adj[u]; g->adj[u] = n1;
    // add v->u (undirected)
    AdjNode *n2 = (AdjNode*)malloc(sizeof(AdjNode));
    n2->v = u; n2->next = g->adj[v]; g->adj[v] = n2;
    int args[2] = { u, v };
    emit(g->cb, "add_edge", args, 2, g->ctx);
}

/* BFS and DFS with emissions of "visit_node" and "edge_traverse" */
void graph_bfs(Graph *g, int start) {
    if (!g->present[start]) return;
    int *visited = (int*)calloc(g->max_nodes, sizeof(int));
    int *queue = (int*)malloc(sizeof(int) * g->max_nodes);
    int qh = 0, qt = 0;
    queue[qt++] = start; visited[start] = 1;
    int args_start[1] = { start }; emit(g->cb, "bfs_start", args_start, 1, g->ctx);
    while (qh < qt) {
        int u = queue[qh++];
        int args_visit[1] = { u }; emit(g->cb, "visit_node", args_visit, 1, g->ctx);
        for (AdjNode *p = g->adj[u]; p; p = p->next) {
            int v = p->v;
            int args_edge[2] = { u, v }; emit(g->cb, "edge_traverse", args_edge, 2, g->ctx);
            if (!visited[v]) {
                visited[v] = 1;
                queue[qt++] = v;
            }
        }
    }
    free(queue); free(visited);
    emit(g->cb, "bfs_end", NULL, 0, g->ctx);
}

static void graph_dfs_rec(Graph *g, int u, int *visited) {
    visited[u] = 1;
    int args_visit[1] = { u }; emit(g->cb, "visit_node", args_visit, 1, g->ctx);
    for (AdjNode *p = g->adj[u]; p; p = p->next) {
        int v = p->v;
        int args_edge[2] = { u, v }; emit(g->cb, "edge_traverse", args_edge, 2, g->ctx);
        if (!visited[v]) graph_dfs_rec(g, v, visited);
    }
}

void graph_dfs(Graph *g, int start) {
    if (!g->present[start]) return;
    int *visited = (int*)calloc(g->max_nodes, sizeof(int));
    int args_start[1] = { start }; emit(g->cb, "dfs_start", args_start, 1, g->ctx);
    graph_dfs_rec(g, start, visited);
    free(visited);
    emit(g->cb, "dfs_end", NULL, 0, g->ctx);
}

void graph_free(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->max_nodes; ++i) {
        AdjNode *p = g->adj[i];
        while (p) {
            AdjNode *tmp = p; p = p->next; free(tmp);
        }
    }
    free(g->adj);
    free(g->present);
    emit(g->cb, "graph_free", NULL, 0, g->ctx);
    free(g);
}


