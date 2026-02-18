#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "ir.h"
#include "codegen.h"

int yyparse(void);
extern FILE *yyin;
extern ASTNode *root;

void printUsage(const char *progName) {
    printf("Usage: %s [OPTIONS] <input_file>\n\n", progName);
    printf("Options:\n");
    printf("  -h, --help          Show this help message\n");
    printf("  -v, --visualize     Visualize algorithm in terminal (default)\n");
    printf("  -j, --json-to-js    Convert JSON to JavaScript for Algorithm Visualizer\n");
    printf("  -o, --output FILE   Specify output file (default: output.json or visualizer.js)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s test.algo                # Compile and visualize in CLI\n", progName);
    printf("  %s -v test.algo              # Same as above\n", progName);
    printf("  %s -j test.algo              # Generate JavaScript for web visualizer\n", progName);
    printf("  %s -j test.algo -o viz.js    # Generate JavaScript with custom output\n", progName);
}

int main(int argc, char **argv) {
    const char *inputPath = NULL;
    const char *outputPath = NULL;
    int visualizeMode = 1;  // Default: CLI visualization
    int jsonToJsMode = 0;
    int errors_found = 0;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--visualize") == 0) {
            visualizeMode = 1;
            jsonToJsMode = 0;
        } else if (strcmp(argv[i], "-j") == 0 || strcmp(argv[i], "--json-to-js") == 0) {
            jsonToJsMode = 1;
            visualizeMode = 0;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                outputPath = argv[++i];
            } else {
                fprintf(stderr, "Error: -o requires an output file name\n");
                return 1;
            }
        } else if (argv[i][0] != '-') {
            inputPath = argv[i];
        } else {
            fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (!inputPath) {
        inputPath = "tests/examples/test.algo";
    }
    
    if (!outputPath) {
        outputPath = jsonToJsMode ? "visualizer.js" : "output.json";
    }

    // Phase 0: File I/O Error Checking
    FILE *in = fopen(inputPath, "r");
    if (!in) {
        fprintf(stderr, "Error: Failed to open input file: %s\n", inputPath);
        fprintf(stderr, "  Check that the file exists and you have read permissions.\n");
        return 1;
    }
    yyin = in;

    printf("Compiling: %s\n", inputPath);
    printf("Phase 1: Lexical Analysis...\n");
    
    resetIR();
    
    // Phase 1: Lexical Analysis (handled by lexer)
    // Phase 2: Syntax Analysis
    printf("Phase 2: Syntax Analysis...\n");
    if (yyparse() != 0) {
        fprintf(stderr, "\nCompilation failed during syntax analysis.\n");
        fclose(in);
        errors_found = 1;
        return 1;
    }
    fclose(in);

    // Phase 3: Semantic Analysis
    printf("Phase 3: Semantic Analysis...\n");
    if (!root) {
        fprintf(stderr, "Error: No valid program structure found.\n");
        return 1;
    }
    
    generateCode(root);
    
    // Check for generation errors
    if (hasGenerationError()) {
        fprintf(stderr, "\nCompilation failed during semantic analysis.\n");
        errors_found = 1;
        return 1;
    }
    
    if (errors_found) {
        return 1;
    }
    
    // Phase 4: Code Generation
    printf("Phase 4: Code Generation...\n");
    writeIRToJSON(outputPath);
    
    if (jsonToJsMode) {
        // Convert JSON to JavaScript
        printf("\nConverting to JavaScript...\n");
        char cmd[512];
        snprintf(cmd, sizeof(cmd), ".\\json_to_js.exe %s %s", outputPath, 
                 strcmp(outputPath, "output.json") == 0 ? "visualizer.js" : outputPath);
        int result = system(cmd);
        if (result == 0) {
            printf("\n[SUCCESS] JavaScript code generated: %s\n", 
                   strcmp(outputPath, "output.json") == 0 ? "visualizer.js" : outputPath);
            printf("  Paste this into Algorithm Visualizer: https://algorithm-visualizer.org/\n");
        } else {
            fprintf(stderr, "\nError: Failed to convert JSON to JavaScript\n");
            return 1;
        }
    } else {
        // CLI Visualization
        printf("\n[SUCCESS] Compilation successful! Output written to %s\n", outputPath);
        printf("\nStarting CLI visualization...\n\n");
        
        char cmd[512];
        snprintf(cmd, sizeof(cmd), ".\\cli_visualizer.exe %s", outputPath);
        system(cmd);
    }
    
    return 0;
}


