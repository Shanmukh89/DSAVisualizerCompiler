#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ir.h"

static IRNode *irHead = NULL;
static IRNode *irTail = NULL;
static char algorithmName[64] = "Algorithm";
static int algorithmArray[256];
static int algorithmArraySize = 0;
static char detectedAlgorithm[64] = "";

void resetIR(void) {
    IRNode *curr = irHead;
    while (curr) {
        IRNode *next = curr->next;
        free(curr);
        curr = next;
    }
    irHead = NULL;
    irTail = NULL;
}

void addIR(const char *action, const char *details) {
    addIRWithIndices(action, details, NULL, 0);
}

void addIRWithIndices(const char *action, const char *details, int *indices, int index_count) {
    IRNode *node = (IRNode *)malloc(sizeof(IRNode));
    if (!node) return;
    memset(node, 0, sizeof(IRNode));
    strncpy(node->action, action ? action : "", sizeof(node->action) - 1);
    strncpy(node->details, details ? details : "", sizeof(node->details) - 1);
    node->index_count = (index_count > 4) ? 4 : index_count;
    if (indices && node->index_count > 0) {
        for (int i = 0; i < node->index_count; i++) {
            node->indices[i] = indices[i];
        }
    }
    node->next = NULL;
    if (!irHead) {
        irHead = node;
        irTail = node;
    } else {
        irTail->next = node;
        irTail = node;
    }
}

void setAlgorithmName(const char *name) {
    if (name) {
        strncpy(algorithmName, name, sizeof(algorithmName) - 1);
        algorithmName[sizeof(algorithmName) - 1] = '\0';
    }
}

void setArray(int *arr, int size) {
    algorithmArraySize = (size > 256) ? 256 : size;
    if (arr) {
        for (int i = 0; i < algorithmArraySize; i++) {
            algorithmArray[i] = arr[i];
        }
    }
}

void setDetectedAlgorithm(const char *name) {
    if (name) {
        strncpy(detectedAlgorithm, name, sizeof(detectedAlgorithm) - 1);
        detectedAlgorithm[sizeof(detectedAlgorithm) - 1] = '\0';
    }
}

const char *getDetectedAlgorithm(void) {
    return detectedAlgorithm;
}

void writeIRToJSON(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Failed to open %s for writing\n", filename);
        return;
    }
    
    // Write new structured JSON format
    fprintf(fp, "{\n");
    fprintf(fp, "  \"algorithm\": \"%s\",\n", algorithmName);
    
    // Write array
    fprintf(fp, "  \"array\": [");
    if (algorithmArraySize > 0) {
        for (int i = 0; i < algorithmArraySize; i++) {
            fprintf(fp, "%d", algorithmArray[i]);
            if (i < algorithmArraySize - 1) fprintf(fp, ", ");
        }
    } else {
        // Default array if not set
        fprintf(fp, "5, 3, 8, 4, 2");
    }
    fprintf(fp, "],\n");
    
    // Write steps
    fprintf(fp, "  \"steps\": [\n");
    IRNode *curr = irHead;
    int step_count = 0;
    while (curr) {
        fprintf(fp, "    {\n");
        fprintf(fp, "      \"action\": \"%s\"", curr->action);
        
        // Write indices if available
        if (curr->index_count > 0) {
            if (curr->index_count == 1) {
                fprintf(fp, ",\n      \"index\": %d", curr->indices[0]);
            } else {
                fprintf(fp, ",\n      \"indices\": [");
                for (int i = 0; i < curr->index_count; i++) {
                    fprintf(fp, "%d", curr->indices[i]);
                    if (i < curr->index_count - 1) fprintf(fp, ", ");
                }
                fprintf(fp, "]");
            }
        }
        
        fprintf(fp, "\n    }");
        if (curr->next) fprintf(fp, ",");
        fprintf(fp, "\n");
        curr = curr->next;
        step_count++;
    }
    fprintf(fp, "  ],\n");
    
    // Write metadata
    fprintf(fp, "  \"metadata\": {\n");
    fprintf(fp, "    \"timeComplexity\": \"O(n²)\",\n");
    fprintf(fp, "    \"spaceComplexity\": \"O(1)\",\n");
    fprintf(fp, "    \"description\": \"Generated from algorithm visualization compiler.\"\n");
    fprintf(fp, "  }\n");
    fprintf(fp, "}\n");
    
    fclose(fp);
}


