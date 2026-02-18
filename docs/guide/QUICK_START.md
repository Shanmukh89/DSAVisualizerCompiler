# Quick Start Guide - How to Run the Compiler

## 📋 Prerequisites

**On Windows:**
- Install [MSYS2](https://www.msys2.org/) or MinGW
- Install tools: `bison`, `flex`, `gcc`
- Or use `win_flex_bison` package

**Required Tools:**
- `bison` or `win_bison` - Parser generator
- `flex` or `win_flex` - Lexer generator  
- `gcc` - C compiler

---

## 🔨 Step 1: Compile Everything

### Option A: Using Makefile (Linux/macOS or MSYS2)

```bash
cd DSAVisualizerCompiler
make
```

This will:
1. Generate `parser.tab.c` and `parser.tab.h` from `parser.y`
2. Generate `lex.yy.c` from `lexer.l`
3. Compile `dsa_compiler` (main compiler)
4. Compile `json_to_js` (JSON to JS translator)

### Option B: Manual Compilation (Windows PowerShell)

If `make` doesn't work, compile manually:

```powershell
# Navigate to directory
cd "C:\Users\shanm\Downloads\Academics\5th Sem\Compiler Design\Cursor\DSAVisualizerCompiler"

# Generate parser (use win_bison if bison not available)
win_bison -d parser.y
# Or: bison -d parser.y

# Generate lexer (use win_flex if flex not available)
win_flex lexer.l
# Or: flex lexer.l

# Compile main compiler
gcc -std=c11 -Wall -Wextra -o dsa_compiler.exe parser.tab.c lex.yy.c ast.c ir.c codegen.c main.c

# Compile JSON to JS translator
gcc -std=c11 -Wall -Wextra -o json_to_js.exe json_to_js.c
```

**Note:** On Windows, if linking with `-lfl` fails, it's already handled - `lexer.l` includes `yywrap()`.

---

## 🚀 Step 2: Run the Complete Pipeline

### Workflow Overview

```
input.algo  →  [Compiler]  →  output.json  →  [Translator]  →  visualizer.js
```

### Step 2.1: Compile Your Algorithm to JSON IR

```powershell
# Run the compiler on your .algo file
.\dsa_compiler.exe test.algo

# This generates: output.json
```

**What it does:**
- Parses your `.algo` file
- Builds an Abstract Syntax Tree (AST)
- Generates Intermediate Representation (IR)
- Outputs structured JSON to `output.json`

**Example input (`test.algo`):**
```
for (i = 0; i < n; i++) {
  if (arr[i] > arr[j]) {
    swap(arr[i], arr[j]);
  }
}
```

**Example output (`output.json`):**
```json
{
  "algorithm": "Bubble Sort",
  "array": [5, 3, 8, 4, 2],
  "steps": [
    {"action": "compare", "indices": [0, 1]},
    {"action": "swap", "indices": [0, 1]},
    ...
  ],
  "metadata": {...}
}
```

### Step 2.2: Convert JSON to JavaScript

```powershell
# Convert the JSON IR to Algorithm Visualizer JavaScript
.\json_to_js.exe output.json visualizer.js

# This generates: visualizer.js
```

**What it does:**
- Reads the structured JSON
- Generates JavaScript code with Algorithm Visualizer tracer API calls
- Outputs ready-to-use JavaScript file

**Example output (`visualizer.js`):**
```javascript
// === Auto-Generated Visualization Script ===
const { Array1DTracer, LogTracer, Layout, VerticalLayout, Tracer } = require('algorithm-visualizer');

const array = [5, 3, 8, 4, 2];
const tracer = new Array1DTracer('Bubble Sort');
const log = new LogTracer('Log');

Layout.setRoot(new VerticalLayout([tracer, log]));
tracer.set(array);
Tracer.delay();

function swap(i, j) {
  const temp = array[i];
  array[i] = array[j];
  array[j] = temp;
  tracer.set(array);
  Tracer.delay();
}

// === Step Playback ===
tracer.select(0, 1);
log.println('Compare 5 and 3');
Tracer.delay();
...
```

### Step 2.3: Use in Algorithm Visualizer

1. Open [Algorithm Visualizer](https://algorithm-visualizer.org/)
2. Go to the editor
3. Copy the entire contents of `visualizer.js`
4. Paste into the editor
5. Click "Run" to see the visualization!

---

## 📝 Complete Example

Here's a complete PowerShell script to run everything:

```powershell
# Navigate to compiler directory
cd "C:\Users\shanm\Downloads\Academics\5th Sem\Compiler Design\Cursor\DSAVisualizerCompiler"

# Step 1: Compile your algorithm
Write-Host "Compiling algorithm..." -ForegroundColor Cyan
.\dsa_compiler.exe test.algo

if (Test-Path output.json) {
    Write-Host "✓ Generated output.json" -ForegroundColor Green
    
    # Step 2: Convert to JavaScript
    Write-Host "Converting to JavaScript..." -ForegroundColor Cyan
    .\json_to_js.exe output.json visualizer.js
    
    if (Test-Path visualizer.js) {
        Write-Host "✓ Generated visualizer.js" -ForegroundColor Green
        Write-Host "`nDone! Copy visualizer.js to Algorithm Visualizer" -ForegroundColor Yellow
    } else {
        Write-Host "✗ Failed to generate visualizer.js" -ForegroundColor Red
    }
} else {
    Write-Host "✗ Failed to compile algorithm" -ForegroundColor Red
}
```

---

## 🔍 Testing with Different Input Files

You can test with different `.algo` files:

```powershell
# Compile a specific file
.\dsa_compiler.exe tests\inputs\case1_assign.algo
.\json_to_js.exe output.json case1_output.js

# Or with custom output names
.\dsa_compiler.exe my_algorithm.algo
.\json_to_js.exe output.json my_visualization.js
```

---

## 🐛 Troubleshooting

### Error: "bison: command not found"
**Solution:** Install `win_flex_bison` or use `win_bison` instead of `bison`

### Error: "flex: command not found"  
**Solution:** Install `win_flex_bison` or use `win_flex` instead of `flex`

### Error: "gcc: command not found"
**Solution:** Install MinGW-w64 or MSYS2, make sure `gcc` is in PATH

### Error: "Failed to open input file"
**Solution:** Check that your `.algo` file exists in the current directory

### Error: "Parse failed"
**Solution:** Check your `.algo` file syntax - make sure it follows the grammar rules

### Linker error with `-lfl`
**Solution:** The `lexer.l` already has `yywrap()` defined, so just remove `-lfl` from the gcc command

---

## 📂 File Structure

```
DSAVisualizerCompiler/
├── test.algo              # Your algorithm input file
├── dsa_compiler.exe       # Main compiler (compile this first)
├── json_to_js.exe         # JSON translator (compile this second)
├── output.json            # Generated JSON IR (intermediate)
├── visualizer.js          # Final JavaScript output (paste to Algorithm Visualizer)
├── parser.y               # Grammar definition
├── lexer.l                # Token definitions
├── ast.h, ast.c           # Abstract Syntax Tree
├── ir.h, ir.c             # Intermediate Representation
├── codegen.c              # Code generation
└── json_to_js.c           # JSON to JS translator
```

---

## ✅ Quick Checklist

- [ ] Install `bison`/`win_bison`, `flex`/`win_flex`, `gcc`
- [ ] Run `make` or manually compile both executables
- [ ] Create/edit your `.algo` file
- [ ] Run `.\dsa_compiler.exe your_file.algo`
- [ ] Run `.\json_to_js.exe output.json visualizer.js`
- [ ] Copy `visualizer.js` contents to Algorithm Visualizer
- [ ] Click Run and watch the visualization!

---

**That's it!** You now have a complete pipeline from algorithm code to visualizable JavaScript! 🎉

