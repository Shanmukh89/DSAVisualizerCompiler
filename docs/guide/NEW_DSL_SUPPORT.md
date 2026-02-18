# New DSL Support - Array Declarations and Algorithm Calls

## ✅ What's Been Added

Your compiler now supports the new DSL syntax:

### 1. Array Declarations
```dsl
arr[]=[2,4,12,3,4];
```

### 2. Algorithm Function Calls
```dsl
bubble_sort(arr);
```

## 📝 Complete DSL Syntax

### Example DSL File (`test_new_dsl.algo`):
```
arr[]=[2,4,12,3,4];
bubble_sort(arr);
```

## 🔧 Changes Made

### 1. AST Updates (`ast.h`, `ast.c`)
- Added `ARRAY_DECL_NODE` and `ALGORITHM_CALL_NODE` node types
- Added `array_values` and `array_count` fields to ASTNode
- Added `createArrayDeclNode()` and `createAlgorithmCallNode()` functions

### 2. Parser Updates (`parser.y`)
- Added grammar rules for `array_decl` and `number_list`
- Updated `call` rule to use `createAlgorithmCallNode()`
- Parser now recognizes:
  - `ID[]=[...]` for array declarations
  - `ID(...)` for algorithm calls

### 3. Code Generator Updates (`codegen.c`)
- Added `collectDSLInfo()` to extract array values and algorithm names from AST
- Updated `generateCode()` to:
  - Use array from DSL if declared
  - Detect algorithm name from function call
  - Map algorithm name to appropriate generation function

### 4. IR Updates (`ir.h`, `ir.c`)
- Added `setDetectedAlgorithm()` and `getDetectedAlgorithm()`
- JSON output uses detected algorithm name

## 🚀 How to Use

### Step 1: Write Your DSL

Create a file with the new syntax:
```
arr[]=[2,4,12,3,4];
bubble_sort(arr);
```

### Step 2: Regenerate Parser

```powershell
win_bison -d parser.y
```

### Step 3: Recompile

```powershell
gcc -std=c11 -Wall -Wextra -o dsa_compiler.exe parser.tab.c lex.yy.c ast.c ir.c codegen.c main.c
```

### Step 4: Run

```powershell
.\dsa_compiler.exe test_new_dsl.algo
.\json_to_js.exe output.json visualizer.js
```

## 📋 Supported Algorithms

Currently supported:
- `bubble_sort(arr)` - Generates bubble sort visualization

Future algorithms can be added:
- `quicksort(arr)`
- `insertion_sort(arr)`
- `selection_sort(arr)`
- etc.

## 🔍 How It Works

1. **Parser** recognizes `arr[]=[2,4,12,3,4]` and creates `ARRAY_DECL_NODE`
2. **Parser** recognizes `bubble_sort(arr)` and creates `ALGORITHM_CALL_NODE`
3. **Codegen** collects:
   - Array values from `ARRAY_DECL_NODE`
   - Algorithm name from `ALGORITHM_CALL_NODE`
4. **Codegen** generates steps using:
   - The array from DSL
   - The appropriate algorithm implementation

## ✨ Benefits

- **Cleaner DSL**: Simple, declarative syntax
- **Automatic Detection**: No need to specify algorithm details
- **Flexible Arrays**: Use any array values you want
- **Extensible**: Easy to add new algorithms

## 🎯 Example Output

**Input (`test_new_dsl.algo`):**
```
arr[]=[2,4,12,3,4];
bubble_sort(arr);
```

**Generated JSON** will use:
- Array: `[2, 4, 12, 3, 4]`
- Algorithm: `"Bubble Sort"`
- All bubble sort steps with the provided array

---

**The compiler now supports your new DSL syntax!** 🎉

