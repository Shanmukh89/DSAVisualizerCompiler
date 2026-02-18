# DSA Visualizer Compiler - Complete Language Reference

This document provides a comprehensive reference for the DSA Visualizer Compiler language, including all syntax definitions, supported features, and usage guidelines.

---

## Table of Contents

1. [Language Overview](#language-overview)
2. [Core Syntax](#core-syntax)
3. [Array Declarations](#array-declarations)
4. [Sorting Algorithms](#sorting-algorithms)
5. [Data Structures](#data-structures)
6. [Control Structures](#control-structures)
7. [Error Handling](#error-handling)
8. [Usage Examples](#usage-examples)
9. [Compiler Phases](#compiler-phases)

---

## Language Overview

The DSA Visualizer Compiler uses a Domain-Specific Language (DSL) designed for visualizing algorithms and data structure operations. The language is compiled through multiple phases to generate visualization-ready output.

### Language Characteristics

- **Syntax**: C-like with simplified constructs
- **Type System**: Primarily integer-based
- **Purpose**: Algorithm and data structure visualization
- **Output Formats**: JSON IR, JavaScript (Algorithm Visualizer), CLI visualization

---

## Core Syntax

### Identifiers

- **Naming Rules**: Start with letter or underscore, followed by letters, digits, or underscores
- **Case Sensitive**: Yes (`bubble_sort` ≠ `Bubble_Sort`)
- **Examples**: `arr`, `i`, `j`, `temp`, `stack`, `queue`

### Literals

- **Integers**: `0`, `1`, `-5`, `42`
- **Arrays**: `[1, 2, 3, 4, 5]`
- **No floating-point**: Only integer values supported

### Comments

Currently, the language does not support comments in the DSL syntax.

---

## Array Declarations

### Syntax

```
array_name[] = [value1, value2, value3, ...];
```

### Examples

```algo
arr[] = [2, 4, 12, 3, 4];
numbers[] = [5, 1, 9, 2, 7];
data[] = [10, 20, 30];
```

### Rules

- Array name must be a valid identifier
- Values must be integers
- Square brackets `[]` required before assignment operator
- Comma-separated values in square brackets
- Must end with semicolon

---

## Sorting Algorithms

### Supported Algorithms

1. **Bubble Sort** - `bubble_sort(array_name);`
2. **Insertion Sort** - `insertion_sort(array_name);`
3. **Selection Sort** - `selection_sort(array_name);`
4. **Merge Sort** - `merge_sort(array_name);`
5. **Quick Sort** - `quick_sort(array_name);`

### Syntax

```
algorithm_name(array_variable);
```

### Complete Example

```algo
arr[] = [5, 3, 8, 2, 1];
bubble_sort(arr);
```

### Algorithm Specifications

#### Bubble Sort

- **Time Complexity**: O(n²)
- **Space Complexity**: O(1)
- **Method**: Repeatedly compares adjacent elements and swaps if out of order

#### Insertion Sort

- **Time Complexity**: O(n²)
- **Space Complexity**: O(1)
- **Method**: Builds sorted array one element at a time

#### Selection Sort

- **Time Complexity**: O(n²)
- **Space Complexity**: O(1)
- **Method**: Finds minimum element and places it at beginning

#### Merge Sort

- **Time Complexity**: O(n log n)
- **Space Complexity**: O(n)
- **Method**: Divide and conquer with guaranteed O(n log n) complexity

#### Quick Sort

- **Time Complexity**: O(n log n) average, O(n²) worst
- **Space Complexity**: O(log n)
- **Method**: Divide and conquer with pivot-based partitioning

---

## Data Structures

### Method Call Syntax

All data structure operations use the method call syntax:

```
data_structure_name.operation_name(arguments);
```

### Stack Operations

**Data Structure**: `stack`

**Operations**:
- `stack.push(value);` - Push element onto stack
- `stack.pop();` - Remove and return top element
- `stack.peek();` - View top element without removing
- `stack.top();` - Alias for `peek()`

**Example**:
```algo
stack.push(10);
stack.push(20);
stack.push(30);
stack.peek();
stack.pop();
stack.pop();
```

**Principle**: LIFO (Last In First Out)

### Queue Operations

**Data Structure**: `queue`

**Operations**:
- `queue.enqueue(value);` - Add element to rear of queue
- `queue.dequeue();` - Remove and return front element

**Example**:
```algo
queue.enqueue(10);
queue.enqueue(20);
queue.enqueue(30);
queue.dequeue();
queue.dequeue();
```

**Principle**: FIFO (First In First Out)

### Binary Tree Operations

**Data Structure**: `tree`

**Operations**:
- `tree.insert(value);` - Insert node with given value
- `tree.traverse_inorder();` - Inorder traversal (Left, Root, Right)
- `tree.traverse_preorder();` - Preorder traversal (Root, Left, Right)
- `tree.traverse_postorder();` - Postorder traversal (Left, Right, Root)

**Example**:
```algo
tree.insert(5);
tree.insert(3);
tree.insert(7);
tree.insert(1);
tree.insert(4);
tree.traverse_inorder();
```

### Graph Operations

**Data Structure**: `graph`

**Operations**:
- `graph.add_node(node_id);` - Add a node to the graph
- `graph.add_edge(from_node, to_node);` - Add edge between two nodes (undirected)
- `graph.bfs(start_node);` - Breadth-First Search starting from given node
- `graph.dfs(start_node);` - Depth-First Search starting from given node

**Example**:
```algo
graph.add_node(1);
graph.add_node(2);
graph.add_node(3);
graph.add_node(4);
graph.add_edge(1, 2);
graph.add_edge(1, 3);
graph.add_edge(2, 4);
graph.bfs(1);
```

---

## Control Structures

### Assignment

**Syntax**:
```
variable = expression;
```

**Examples**:
```algo
x = 5;
i = 0;
arr[i] = 10;
```

### For Loops

**Syntax**:
```
for (init; condition; increment) {
    statements
}
```

**Example**:
```algo
for (i = 0; i < n; i++) {
    // statements
}
```

**Components**:
- `init`: Initialization expression (optional, but typically assignment)
- `condition`: Loop condition (comparison expression)
- `increment`: Increment/decrement expression (typically `i++`)

### If Statements

**Syntax**:
```
if (condition) {
    statements
}
```

**Example**:
```algo
if (arr[i] > arr[j]) {
    swap(arr[i], arr[j]);
}
```

**Conditions**:
- Comparison operators: `<`, `>`, `<=`, `>=`, `==`, `!=`
- Currently supports simple comparison expressions

### Function Calls

**Syntax**:
```
function_name(arguments);
```

**Examples**:
```algo
swap(arr[i], arr[j]);
bubble_sort(arr);
stack.push(10);
```

**Arguments**:
- Zero or more comma-separated expressions
- Can be variables, literals, or array accesses

---

## Error Handling

The compiler implements comprehensive error checking at multiple phases:

### Lexical Errors

**Detection**: Invalid characters in source code

**Example Error**:
```
Lexical Error: Invalid character '#' at line 3, column 5
  Unexpected character encountered. Valid characters include: letters, numbers, operators, and punctuation.
```

### Syntax Errors

**Detection**: Invalid grammar constructions

**Example Error**:
```
Syntax Error at line 2: syntax error, unexpected ';', expecting ID
```

**Common Causes**:
- Missing semicolons
- Mismatched parentheses or brackets
- Invalid operator usage
- Missing or extra tokens

### Semantic Errors

**Detection**: Invalid algorithm/data structure names, invalid operations

#### Algorithm Name Validation

**Valid Algorithms**:
- `bubble_sort`
- `insertion_sort`
- `selection_sort`
- `merge_sort`
- `quick_sort` or `quicksort`

**Example Error**:
```
Semantic Error at line 2: Unknown algorithm: 'bule_sort'. Valid algorithms are: bubble_sort, insertion_sort, selection_sort, merge_sort, quick_sort
Did you mean: bubble_sort?
```

#### Data Structure Validation

**Valid Data Structures**:
- `stack`
- `queue`
- `tree`
- `graph`

**Example Error**:
```
Semantic Error at line 1: Unknown data structure: 'stak'. Valid options are: stack, queue, tree, graph
```

#### Operation Validation

**Stack Operations**:
- Valid: `push`, `pop`, `peek`, `top`
- Invalid: `pus`, `add`, `remove`, etc.

**Example Error**:
```
Semantic Error at line 1: Invalid stack operation: 'pus'. Valid operations are: push, pop, peek, top
```

**Queue Operations**:
- Valid: `enqueue`, `dequeue`

**Tree Operations**:
- Valid: `insert`, `traverse_inorder`, `traverse_preorder`, `traverse_postorder`

**Graph Operations**:
- Valid: `add_node`, `add_edge`, `bfs`, `dfs`

### Error Reporting Features

1. **Line Numbers**: Errors include line number information
2. **Column Numbers**: Lexical errors include column information
3. **Suggestions**: Semantic errors may include suggestions (e.g., "Did you mean: bubble_sort?")
4. **Valid Options**: Error messages list all valid alternatives
5. **Early Exit**: Compilation stops on first error

---

## Usage Examples

### Example 1: Bubble Sort

**File**: `bubble_sort_example.algo`

```algo
arr[] = [5, 3, 8, 2, 1];
bubble_sort(arr);
```

**Compilation**:
```bash
.\dsa_compiler.exe bubble_sort_example.algo
```

### Example 2: Stack Operations

**File**: `stack_example.algo`

```algo
stack.push(10);
stack.push(20);
stack.push(30);
stack.peek();
stack.pop();
stack.pop();
```

**Compilation**:
```bash
.\dsa_compiler.exe stack_example.algo
```

### Example 3: Queue Operations

**File**: `queue_example.algo`

```algo
queue.enqueue(10);
queue.enqueue(20);
queue.enqueue(30);
queue.dequeue();
queue.dequeue();
```

### Example 4: Tree Traversal

**File**: `tree_example.algo`

```algo
tree.insert(5);
tree.insert(3);
tree.insert(7);
tree.insert(1);
tree.insert(4);
tree.traverse_inorder();
```

### Example 5: Graph BFS

**File**: `graph_example.algo`

```algo
graph.add_node(1);
graph.add_node(2);
graph.add_node(3);
graph.add_node(4);
graph.add_edge(1, 2);
graph.add_edge(1, 3);
graph.add_edge(2, 4);
graph.bfs(1);
```

---

## Compiler Phases

The compiler processes source code through multiple phases:

### Phase 1: Lexical Analysis

**Purpose**: Tokenize source code into tokens

**Process**:
- Reads source file character by character
- Groups characters into tokens (identifiers, numbers, operators)
- Recognizes keywords and special symbols
- Reports lexical errors for invalid characters

**Output**: Stream of tokens

### Phase 2: Syntax Analysis

**Purpose**: Parse tokens into Abstract Syntax Tree (AST)

**Process**:
- Uses grammar rules (Yacc/Bison)
- Constructs parse tree representing program structure
- Validates syntax according to language grammar
- Reports syntax errors for invalid constructions

**Output**: Abstract Syntax Tree (AST)

### Phase 3: Semantic Analysis

**Purpose**: Validate program semantics and collect information

**Process**:
- Traverses AST
- Validates algorithm/data structure names
- Validates operation names
- Checks for semantic inconsistencies
- Reports semantic errors

**Output**: Validated AST with semantic information

### Phase 4: Code Generation

**Purpose**: Generate Intermediate Representation (IR)

**Process**:
- Traverses validated AST
- Simulates algorithm execution
- Generates step-by-step visualization data
- Creates JSON IR format

**Output**: JSON file with visualization steps

---

## Compiler Command-Line Interface

### Basic Usage

```bash
dsa_compiler.exe [OPTIONS] <input_file>
```

### Options

- `-h, --help` - Show help message
- `-v, --visualize` - Visualize algorithm in terminal (default mode)
- `-j, --json-to-js` - Convert JSON to JavaScript for Algorithm Visualizer
- `-o, --output FILE` - Specify output file (default: `output.json` or `visualizer.js`)

### Examples

```bash
# CLI Visualization (default)
.\dsa_compiler.exe test.algo
.\dsa_compiler.exe -v test.algo

# Generate JavaScript for web
.\dsa_compiler.exe -j test.algo
.\dsa_compiler.exe -j test.algo -o visualizer.js

# Custom output file
.\dsa_compiler.exe test.algo -o my_output.json
```

---

## Output Formats

### JSON Intermediate Representation

The compiler generates JSON files with the following structure:

```json
{
  "algorithm": "Bubble Sort",
  "array": [5, 3, 8, 2, 1],
  "steps": [
    {
      "action": "compare",
      "indices": [0, 1]
    },
    {
      "action": "swap",
      "indices": [0, 1]
    },
    {
      "action": "mark_sorted",
      "index": 4
    }
  ],
  "metadata": {
    "timeComplexity": "O(n²)",
    "spaceComplexity": "O(1)",
    "description": "Generated from algorithm visualization compiler."
  }
}
```

### JavaScript Output (Algorithm Visualizer)

The JSON can be converted to JavaScript code compatible with Algorithm Visualizer:

```javascript
const { Array1DTracer, LogTracer, Layout, VerticalLayout, Tracer } = require('algorithm-visualizer');

const array = [5, 3, 8, 2, 1];
const tracer = new Array1DTracer('Bubble Sort');
const log = new LogTracer('Log');

Layout.setRoot(new VerticalLayout([tracer, log]));
tracer.set(array);
Tracer.delay();

// Step-by-step visualization code...
```

### CLI Visualization

The compiler can also generate interactive terminal-based visualization with:
- Step-by-step algorithm execution
- Array state visualization
- Comparison and swap highlighting
- Algorithm-specific descriptions
- Statistics tracking (comparisons, swaps, passes)

---

## Language Grammar Summary

### EBNF-like Grammar (Simplified)

```
program          ::= statement_list

statement_list   ::= statement | statement_list statement

statement        ::= assignment SEMICOLON
                  | array_decl SEMICOLON
                  | algorithm_call SEMICOLON
                  | method_call SEMICOLON
                  | loop
                  | condition

assignment       ::= ID ASSIGN expr
                  | ID LBRACKET expr RBRACKET ASSIGN expr

array_decl       ::= ID LBRACKET RBRACKET ASSIGN array_values

array_values     ::= LBRACKET number_list RBRACKET

number_list      ::= NUMBER | number_list COMMA NUMBER

algorithm_call   ::= ID LPAREN opt_args RPAREN

method_call      ::= ID DOT ID LPAREN opt_args RPAREN

opt_args         ::= /* empty */ | arg_list

arg_list         ::= expr | arg_list COMMA expr

loop             ::= FOR LPAREN for_init SEMICOLON expr SEMICOLON for_inc RPAREN LBRACE opt_stmt_list RBRACE

condition        ::= IF LPAREN expr RPAREN LBRACE opt_stmt_list RBRACE

expr             ::= NUMBER
                  | ID
                  | ID LBRACKET expr RBRACKET
                  | expr PLUS expr
                  | expr MINUS expr
                  | expr MULTIPLY expr
                  | expr DIVIDE expr
                  | expr LT expr
                  | expr GT expr
                  | expr LE expr
                  | expr GE expr
                  | expr EQ expr
                  | expr NE expr
                  | LPAREN expr RPAREN
```

---

## Reserved Keywords

While the language does not have traditional reserved keywords, the following identifiers have special meaning:

- Algorithm names: `bubble_sort`, `insertion_sort`, `selection_sort`, `merge_sort`, `quick_sort`
- Data structure names: `stack`, `queue`, `tree`, `graph`
- Control flow: `for`, `if`

---

## Limitations and Future Enhancements

### Current Limitations

1. **No Comments**: Comments are not currently supported
2. **Integer Only**: No floating-point or string types
3. **Fixed Array Size**: Arrays must be declared with explicit values
4. **Limited Operators**: Basic arithmetic and comparison operators only
5. **No Functions**: Cannot define custom functions
6. **No Variables in Array Declarations**: Array values must be literals

### Potential Future Enhancements

- Support for comments (`//` and `/* */`)
- Floating-point number support
- String type and operations
- Variable-size arrays
- Custom function definitions
- More data structures (heap, hash table, linked list)
- More algorithms (advanced graph algorithms, string algorithms)
- Type system with type checking
- Optimizations and performance analysis

---

## Quick Reference Card

### Array Declaration
```algo
arr[] = [1, 2, 3];
```

### Sorting Algorithms
```algo
bubble_sort(arr);
insertion_sort(arr);
selection_sort(arr);
merge_sort(arr);
quick_sort(arr);
```

### Stack
```algo
stack.push(value);
stack.pop();
stack.peek();
```

### Queue
```algo
queue.enqueue(value);
queue.dequeue();
```

### Tree
```algo
tree.insert(value);
tree.traverse_inorder();
```

### Graph
```algo
graph.add_node(id);
graph.add_edge(from, to);
graph.bfs(start);
```

---

## Conclusion

This document provides a complete reference for the DSA Visualizer Compiler language. For additional documentation, see the `docs/guide/` folder for detailed guides on specific topics.

For questions or issues, refer to the error messages which provide detailed information about what went wrong and how to fix it.

