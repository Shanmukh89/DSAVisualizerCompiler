#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 1024
#define MAX_ACTION_LEN 32
#define MAX_ARRAY_SIZE 256

typedef struct Step {
    char action[MAX_ACTION_LEN];
    int indices[4];
    int index_count;
    struct Step *next;
} Step;

typedef struct {
    char algorithm[64];
    int array[MAX_ARRAY_SIZE];
    int array_size;
    Step *steps;
} JSONData;

static void freeSteps(Step *head) {
    while (head) {
        Step *next = head->next;
        free(head);
        head = next;
    }
}

static int parseNumber(const char *str, int *value) {
    if (!str || !value) return 0;
    char *end;
    *value = (int)strtol(str, &end, 10);
    return (end != str && *end == '\0');
}

static int extractString(const char *line, const char *key, char *output, size_t max_len) {
    char search_str[128];
    snprintf(search_str, sizeof(search_str), "\"%s\"", key);
    char *pos = strstr(line, search_str);
    if (!pos) return 0;
    
    pos = strchr(pos, ':');
    if (!pos) return 0;
    pos++;  // Skip ':'
    
    // Skip whitespace
    while (*pos && isspace(*pos)) pos++;
    
    // Check for string
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

static int extractArray(const char *line, int *arr, int max_size, int *actual_size) {
    char *pos = strchr(line, '[');
    if (!pos) return 0;
    pos++;
    
    *actual_size = 0;
    while (*pos && *pos != ']' && *actual_size < max_size) {
        // Skip whitespace and commas
        while (*pos && (isspace(*pos) || *pos == ',')) pos++;
        if (*pos == ']') break;
        
        int value;
        char *end;
        value = (int)strtol(pos, &end, 10);
        if (end == pos) break; // Not a number
        
        arr[*actual_size] = value;
        (*actual_size)++;
        pos = end;
    }
    return (*actual_size > 0);
}

static int extractIndices(const char *line, int *indices, int *count) {
    *count = 0;
    
    // Try "indices": [x, y]
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
    
    // Try "index": x
    char *index_pos = strstr(line, "\"index\"");
    if (index_pos) {
        index_pos = strchr(index_pos, ':');
        if (index_pos) {
            index_pos++;
            while (*index_pos && isspace(*index_pos)) index_pos++;
            int value;
            if (parseNumber(index_pos, &value)) {
                indices[0] = value;
                *count = 1;
                return 1;
            }
        }
    }
    
    return 0;
}

static JSONData *parseJSON(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open %s\n", filename);
        return NULL;
    }
    
    JSONData *data = (JSONData *)calloc(1, sizeof(JSONData));
    if (!data) {
        fclose(fp);
        return NULL;
    }
    
    strcpy(data->algorithm, "Algorithm");
    data->array_size = 0;
    data->steps = NULL;
    
    // Read entire file into buffer for easier parsing
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer) {
        fclose(fp);
        free(data);
        return NULL;
    }
    fread(buffer, 1, file_size, fp);
    buffer[file_size] = '\0';
    
    // Extract algorithm name
    char alg_name[64];
    if (extractString(buffer, "algorithm", alg_name, sizeof(alg_name))) {
        strncpy(data->algorithm, alg_name, sizeof(data->algorithm) - 1);
    }
    
    // Extract array
    if (strstr(buffer, "\"array\"")) {
        extractArray(buffer, data->array, MAX_ARRAY_SIZE, &data->array_size);
    }
    
    // Extract steps - find steps array
    char *steps_start = strstr(buffer, "\"steps\"");
    if (steps_start) {
        steps_start = strchr(steps_start, '[');
        if (steps_start) {
            steps_start++;  // Skip '['
            char *steps_end = strrchr(steps_start, ']');
            if (steps_end) {
                // Process steps between [ and ]
                char *step_pos = steps_start;
                Step *step_tail = NULL;
                
                while (step_pos < steps_end) {
                    // Skip whitespace and commas
                    while (step_pos < steps_end && (isspace(*step_pos) || *step_pos == ',')) step_pos++;
                    if (step_pos >= steps_end) break;
                    
                    // Find next step object
                    char *obj_start = strchr(step_pos, '{');
                    if (!obj_start || obj_start >= steps_end) break;
                    
                    // Find matching closing brace
                    char *obj_end = obj_start + 1;
                    int brace_count = 1;
                    while (obj_end < steps_end && brace_count > 0) {
                        if (*obj_end == '{') brace_count++;
                        else if (*obj_end == '}') brace_count--;
                        obj_end++;
                    }
                    if (brace_count != 0 || obj_end > steps_end) break;
                    
                    // Create temporary string for this step object
                    size_t obj_len = obj_end - obj_start + 1;
                    char *step_obj = (char *)malloc(obj_len + 1);
                    if (step_obj) {
                        strncpy(step_obj, obj_start, obj_len);
                        step_obj[obj_len] = '\0';
                        
                        Step *step = (Step *)calloc(1, sizeof(Step));
                        if (step) {
                            // Extract action
                            char action[64];
                            if (extractString(step_obj, "action", action, sizeof(action))) {
                                strncpy(step->action, action, sizeof(step->action) - 1);
                                step->action[sizeof(step->action) - 1] = '\0';
                            }
                            
                            // Extract indices
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
                    
                    step_pos = obj_end + 1;
                }
            }
        }
    }
    
    free(buffer);
    
    fclose(fp);
    return data;
}

static void generateJS(JSONData *data, FILE *out) {
    if (!data) return;
    
    // Determine data structure type from algorithm name
    int is_stack = (strstr(data->algorithm, "Stack") != NULL);
    int is_queue = (strstr(data->algorithm, "Queue") != NULL);
    int is_tree = (strstr(data->algorithm, "Tree") != NULL);
    int is_graph = (strstr(data->algorithm, "Graph") != NULL);
    
    // Write header with imports
    fprintf(out, "// === Auto-Generated Visualization Script ===\n");
    if (is_tree || is_graph) {
        fprintf(out, "const { Array1DTracer, GraphTracer, LogTracer, Layout, VerticalLayout, HorizontalLayout, Tracer } = require('algorithm-visualizer');\n\n");
    } else {
        fprintf(out, "const { Array1DTracer, LogTracer, Layout, VerticalLayout, Tracer } = require('algorithm-visualizer');\n\n");
    }
    
    // Write array initialization
    fprintf(out, "const array = [");
    for (int i = 0; i < data->array_size; i++) {
        fprintf(out, "%d", data->array[i]);
        if (i < data->array_size - 1) fprintf(out, ", ");
    }
    fprintf(out, "];\n");
    
    // Write tracer initialization based on data structure type
    if (is_tree || is_graph) {
        fprintf(out, "const tracer = new GraphTracer('%s');\n", data->algorithm);
        fprintf(out, "const log = new LogTracer('Log');\n\n");
        fprintf(out, "Layout.setRoot(new VerticalLayout([tracer, log]));\n");
        // For trees/graphs, we'll set nodes dynamically
    } else {
        fprintf(out, "const tracer = new Array1DTracer('%s');\n", data->algorithm);
        fprintf(out, "const log = new LogTracer('Log');\n\n");
        fprintf(out, "Layout.setRoot(new VerticalLayout([tracer, log]));\n");
        fprintf(out, "tracer.set(array);\n");
    }
    fprintf(out, "Tracer.delay();\n\n");
    
    // Write swap function
    fprintf(out, "function swap(i, j) {\n");
    fprintf(out, "  const temp = array[i];\n");
    fprintf(out, "  array[i] = array[j];\n");
    fprintf(out, "  array[j] = temp;\n");
    fprintf(out, "  tracer.set(array);\n");
    fprintf(out, "  Tracer.delay();\n");
    fprintf(out, "}\n\n");
    
    // Write step playback
    fprintf(out, "// === Step Playback ===\n");
    
    Step *curr = data->steps;
    
    while (curr) {
        if (strcmp(curr->action, "compare") == 0) {
            if (curr->index_count >= 2) {
                int i = curr->indices[0];
                int j = curr->indices[1];
                
                // Get array values for logging
                int val_i = (i < data->array_size) ? data->array[i] : 0;
                int val_j = (j < data->array_size) ? data->array[j] : 0;
                
                fprintf(out, "tracer.select(%d, %d);\n", i, j);
                fprintf(out, "log.println('Compare %d and %d');\n", val_i, val_j);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "swap") == 0) {
            if (curr->index_count >= 2) {
                int i = curr->indices[0];
                int j = curr->indices[1];
                
                int val_i = (i < data->array_size) ? data->array[i] : 0;
                int val_j = (j < data->array_size) ? data->array[j] : 0;
                
                fprintf(out, "log.println('Swapping %d and %d');\n", val_i, val_j);
                fprintf(out, "swap(%d, %d);\n", i, j);
                
                // Update array state for future references
                int temp = data->array[i];
                data->array[i] = data->array[j];
                data->array[j] = temp;
                
                fprintf(out, "tracer.deselect(%d, %d);\n", i, j);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "no_swap") == 0) {
            if (curr->index_count >= 2) {
                int i = curr->indices[0];
                int j = curr->indices[1];
                
                fprintf(out, "log.println('No swap needed');\n");
                fprintf(out, "tracer.deselect(%d, %d);\n", i, j);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "mark_sorted") == 0) {
            if (curr->index_count >= 1) {
                int idx = curr->indices[0];
                fprintf(out, "log.println('✅ Element at index %d sorted');\n", idx);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "push") == 0) {
            if (curr->index_count >= 2) {
                int pos = curr->indices[0];
                int val = curr->indices[1];
                fprintf(out, "log.println('Pushing %d onto stack');\n", val);
                fprintf(out, "tracer.patch(%d, %d);\n", pos, val);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "pop") == 0) {
            if (curr->index_count >= 2) {
                int pos = curr->indices[0];
                int val = curr->indices[1];
                fprintf(out, "log.println('Popping %d from stack');\n", val);
                fprintf(out, "tracer.depatch(%d);\n", pos);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "peek") == 0) {
            if (curr->index_count >= 1) {
                int pos = curr->indices[0];
                fprintf(out, "log.println('Peeking at top of stack');\n");
                fprintf(out, "tracer.select(%d);\n", pos);
                fprintf(out, "Tracer.delay();\n");
                fprintf(out, "tracer.deselect(%d);\n", pos);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "enqueue") == 0) {
            if (curr->index_count >= 2) {
                int pos = curr->indices[0];
                int val = curr->indices[1];
                fprintf(out, "log.println('Enqueuing %d');\n", val);
                fprintf(out, "tracer.patch(%d, %d);\n", pos, val);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "dequeue") == 0) {
            if (curr->index_count >= 2) {
                int pos = curr->indices[0];
                int val = curr->indices[1];
                fprintf(out, "log.println('Dequeuing %d');\n", val);
                fprintf(out, "tracer.depatch(%d);\n", pos);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "insert") == 0) {
            if (curr->index_count >= 4) {
                int val = curr->indices[0];
                int parent = curr->indices[1];
                int is_left = curr->indices[2];
                if (parent == -1) {
                    fprintf(out, "log.println('Inserting %d as root');\n", val);
                    fprintf(out, "tracer.setTreeData({ '%d': { value: %d } });\n", val, val);
                } else {
                    fprintf(out, "log.println('Inserting %d as %s child of %d');\n", 
                        val, is_left ? "left" : "right", parent);
                    fprintf(out, "tracer.setTreeData({ '%d': { value: %d, parent: '%d', left: %s } });\n", 
                        val, val, parent, is_left ? "true" : "false");
                }
                fprintf(out, "Tracer.delay();\n\n");
            } else if (curr->index_count >= 1) {
                int val = curr->indices[0];
                fprintf(out, "log.println('Inserting node %d');\n", val);
                fprintf(out, "tracer.select(%d);\n", val);
                fprintf(out, "Tracer.delay();\n");
                fprintf(out, "tracer.deselect(%d);\n", val);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "visit") == 0) {
            if (curr->index_count >= 1) {
                int val = curr->indices[0];
                fprintf(out, "log.println('Visiting node: %d');\n", val);
                fprintf(out, "tracer.select(%d);\n", val);
                fprintf(out, "Tracer.delay();\n");
                fprintf(out, "tracer.deselect(%d);\n", val);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "add_node") == 0) {
            if (curr->index_count >= 1) {
                int node = curr->indices[0];
                fprintf(out, "log.println('Adding node %d');\n", node);
                fprintf(out, "tracer.addNode(%d);\n", node);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "add_edge") == 0) {
            if (curr->index_count >= 2) {
                int from = curr->indices[0];
                int to = curr->indices[1];
                fprintf(out, "log.println('Adding edge from %d to %d');\n", from, to);
                fprintf(out, "tracer.addEdge(%d, %d);\n", from, to);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        else if (strcmp(curr->action, "bfs_start") == 0 || strcmp(curr->action, "dfs_start") == 0) {
            if (curr->index_count >= 1) {
                int start = curr->indices[0];
                fprintf(out, "log.println('Starting %s from node %d');\n", 
                    strcmp(curr->action, "bfs_start") == 0 ? "BFS" : "DFS", start);
                fprintf(out, "tracer.select(%d);\n", start);
                fprintf(out, "Tracer.delay();\n\n");
            }
        }
        
        curr = curr->next;
    }
    
    fprintf(out, "log.println('Done!');\n");
}

int main(int argc, char **argv) {
    const char *inputFile = (argc > 1) ? argv[1] : "output.json";
    const char *outputFile = (argc > 2) ? argv[2] : "visualizer.js";
    
    JSONData *data = parseJSON(inputFile);
    if (!data) {
        fprintf(stderr, "Failed to parse JSON IR\n");
        return 1;
    }
    
    FILE *out = fopen(outputFile, "w");
    if (!out) {
        fprintf(stderr, "Failed to open %s for writing\n", outputFile);
        freeSteps(data->steps);
        free(data);
        return 1;
    }
    
    generateJS(data, out);
    fclose(out);
    
    freeSteps(data->steps);
    free(data);
    
    printf("JavaScript visualization code generated: %s\n", outputFile);
    return 0;
}
