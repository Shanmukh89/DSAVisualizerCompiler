# Multi-Phase Error Handling

The compiler now includes comprehensive error checking at **each phase** of compilation, just like real compilers (GCC, Clang, etc.).

## Compilation Phases

### Phase 0: File I/O Error Checking
- **Location**: `main.c`
- **Checks**: File existence, read permissions
- **Error Example**:
  ```
  Error: Failed to open input file: nonexistent.algo
    Check that the file exists and you have read permissions.
  ```

### Phase 1: Lexical Analysis (Lexer)
- **Location**: `lexer.l`
- **Checks**: Invalid characters, malformed tokens
- **Features**:
  - Line and column tracking
  - Detailed error messages with location
- **Error Example**:
  ```
  Lexical Error: Invalid character '#' at line 3, column 1
    Unexpected character encountered. Valid characters include: letters, numbers, operators, and punctuation.
  ```

### Phase 2: Syntax Analysis (Parser)
- **Location**: `parser.y`
- **Checks**: Grammar violations, missing tokens, unexpected tokens
- **Features**:
  - Line number reporting
  - Expected token information
  - Context-aware error messages
- **Error Example**:
  ```
  Syntax Error at line 2: syntax error, unexpected ID, expecting SEMICOLON
    Unexpected token: 'bubble_sort'
    Expected: statement, declaration, or expression
  ```

### Phase 3: Semantic Analysis (Code Generator)
- **Location**: `codegen.c`
- **Checks**:
  - Invalid algorithm names
  - Invalid data structure operations
  - Missing required arguments
  - Undefined identifiers
- **Features**:
  - Line number tracking
  - Suggestions for corrections
  - Validation of all operations
- **Error Examples**:
  ```
  Semantic Error: Unknown algorithm: 'bube_sort'. Valid algorithms are: bubble_sort, insertion_sort, selection_sort, merge_sort, quick_sort
  Did you mean: bubble_sort?
  ```

  ```
  Semantic Error at line 2: Invalid stack operation: 'pus'. Valid operations are: push, pop, peek, top
  ```

### Phase 4: Code Generation
- **Location**: `codegen.c` → `ir.c`
- **Action**: Generate JSON IR only if all previous phases pass
- **Success Message**:
  ```
  ✓ Compilation successful! Output written to output.json
  ```

---

## Error Flow

```
Input File
    ↓
[Phase 0] File I/O Check
    ↓ (if error: exit)
[Phase 1] Lexical Analysis
    ↓ (if error: exit)
[Phase 2] Syntax Analysis
    ↓ (if error: exit)
[Phase 3] Semantic Analysis
    ↓ (if error: exit)
[Phase 4] Code Generation
    ↓
Output JSON
```

---

## Error Detection Summary

### Lexical Errors
- Invalid characters (e.g., `#`, `@`, `$`)
- Malformed number literals
- Unrecognized tokens

### Syntax Errors
- Missing semicolons
- Unmatched parentheses/brackets
- Invalid statement structure
- Unexpected tokens

### Semantic Errors
- Invalid algorithm names (`bule_sort`, `bube_sort`)
- Invalid data structure types (`stak`, `queu`)
- Invalid operations (`pus`, `enqueu`)
- Missing required arguments
- Undefined identifiers
- Type mismatches

---

## Example Test Cases

### Valid Input ✅
```algo
arr[]=[2,4,12,3,4];
bubble_sort(arr);
```
**Output**: `✓ Compilation successful!`

### Lexical Error ❌
```algo
arr[]=[2,4,12,3,4];
bubble_sort(arr);
# Invalid comment
```
**Output**: 
```
Lexical Error: Invalid character '#' at line 3, column 1
```

### Syntax Error ❌
```algo
arr[]=[2,4,12,3,4]
bubble_sort(arr);
```
**Output**:
```
Syntax Error at line 2: syntax error, unexpected ID, expecting SEMICOLON
```

### Semantic Error ❌
```algo
arr[]=[2,4,12,3,4];
bube_sort(arr);
```
**Output**:
```
Semantic Error: Unknown algorithm: 'bube_sort'. Valid algorithms are: bubble_sort, insertion_sort, selection_sort, merge_sort, quick_sort
Did you mean: bubble_sort?
```

---

## Implementation Details

### Lexer Error Handling
- Tracks `yylineno` (line number)
- Tracks `column` position
- Reports invalid characters with location
- Returns error token to parser

### Parser Error Handling
- Uses `%error-verbose` for detailed messages
- Tracks current token (`yytext`)
- Reports expected tokens
- Provides context in error messages

### Semantic Error Handling
- Validates all identifiers (algorithms, data structures, operations)
- Checks argument presence and types
- Provides helpful suggestions
- Stops on first error (early exit)

---

## Compiler Output Format

### Successful Compilation
```
Compiling: test.algo
Phase 1: Lexical Analysis...
Phase 2: Syntax Analysis...
Phase 3: Semantic Analysis...
Phase 4: Code Generation...

✓ Compilation successful! Output written to output.json
```

### Failed Compilation
```
Compiling: test_error.algo
Phase 1: Lexical Analysis...
Phase 2: Syntax Analysis...

[Error message appears here]

Compilation failed during [phase name].
```

---

## Benefits

1. **Early Error Detection**: Errors caught at the earliest possible phase
2. **Clear Messages**: Each error includes location and context
3. **No False Positives**: Valid code compiles successfully
4. **No False Negatives**: Invalid code is always caught
5. **Professional Behavior**: Mimics real compiler error handling
6. **Helpful Suggestions**: Provides corrections when possible

---

## Comparison: Before vs After

### Before
- Errors silently ignored
- Invalid code produced output
- No phase-specific detection
- Generic error messages

### After
- Errors caught at appropriate phase
- Invalid code rejected
- Phase-specific detection
- Detailed, contextual error messages
- Line/column information
- Helpful suggestions

---

The compiler now behaves like a professional tool with comprehensive error checking at every phase! 🎉

