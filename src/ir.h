#ifndef IR_H
#define IR_H

typedef struct IRNode {
    char action[32];
    char details[128];
    int indices[4];  // Support up to 4 indices for actions
    int index_count; // Number of valid indices
    struct IRNode *next;
} IRNode;

void resetIR(void);
void addIR(const char *action, const char *details);
void addIRWithIndices(const char *action, const char *details, int *indices, int index_count);
void setAlgorithmName(const char *name);
void setArray(int *arr, int size);
void setDetectedAlgorithm(const char *name);
const char *getDetectedAlgorithm(void);
void writeIRToJSON(const char *filename);

#endif


