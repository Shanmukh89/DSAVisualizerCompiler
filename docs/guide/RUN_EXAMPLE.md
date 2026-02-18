 # How to Run - Step by Step Example

## 🎯 Simple Example Run

Here's exactly what to do:

### Step 1: Navigate to the Compiler Directory

```powershell
cd "C:\Users\shanm\Downloads\Academics\5th Sem\Compiler Design\Cursor\DSAVisualizerCompiler"
```

### Step 2: Compile the Tools (First Time Only)

If you haven't compiled yet, you need to build both executables:

```powershell
# Generate parser files
win_bison -d parser.y
win_flex lexer.l

# Compile the main compiler
gcc -std=c11 -Wall -Wextra -o dsa_compiler.exe parser.tab.c lex.yy.c ast.c ir.c codegen.c main.c

# Compile the JSON translator
gcc -std=c11 -Wall -Wextra -o json_to_js.exe json_to_js.c
```

**After first time:** You only need to recompile if you changed the source code.

### Step 3: Run the Complete Pipeline

#### 3a. Compile Your Algorithm to JSON

```powershell
.\dsa_compiler.exe test.algo
```

**Output:** Creates `output.json`

**What happens:**
- Reads `test.algo`
- Parses the algorithm
- Generates JSON with steps, actions, and indices

#### 3b. Convert JSON to JavaScript

```powershell
.\json_to_js.exe output.json visualizer.js
```

**Output:** Creates `visualizer.js`

**What happens:**
- Reads `output.json`
- Generates Algorithm Visualizer JavaScript code
- Includes all tracer API calls, logging, and animations

### Step 4: View the Results

**Check the generated JSON:**
```powershell
Get-Content output.json
```

**Check the generated JavaScript:**
```powershell
Get-Content visualizer.js
```

### Step 5: Use in Algorithm Visualizer

1. Open https://algorithm-visualizer.org/
2. Click on any algorithm editor
3. Select all existing code and delete it
4. Copy all contents from `visualizer.js`
5. Paste into the editor
6. Click the "Run" button
7. Watch the visualization! 🎉

---

## 📋 Complete Workflow Diagram

```
test.algo
    ↓
[dsa_compiler.exe]
    ↓
output.json
    ↓
[json_to_js.exe]
    ↓
visualizer.js
    ↓
[Algorithm Visualizer Website]
    ↓
✨ Beautiful Visualization!
```

---

## 🔄 Quick Run Script

Save this as `run_complete.ps1` in the DSAVisualizerCompiler directory:

```powershell
# Complete Pipeline Runner
$inputFile = if ($args.Count -gt 0) { $args[0] } else { "test.algo" }
$outputJS = if ($args.Count -gt 1) { $args[1] } else { "visualizer.js" }

Write-Host "=== DSA Visualizer Compiler Pipeline ===" -ForegroundColor Cyan
Write-Host ""

Write-Host "Step 1: Compiling algorithm '$inputFile'..." -ForegroundColor Yellow
.\dsa_compiler.exe $inputFile

if (Test-Path "output.json") {
    Write-Host "  ✓ Generated output.json" -ForegroundColor Green
    
    Write-Host "Step 2: Converting to JavaScript..." -ForegroundColor Yellow
    .\json_to_js.exe output.json $outputJS
    
    if (Test-Path $outputJS) {
        Write-Host "  ✓ Generated $outputJS" -ForegroundColor Green
        Write-Host ""
        Write-Host "=== SUCCESS! ===" -ForegroundColor Green
        Write-Host "Copy the contents of '$outputJS' to Algorithm Visualizer" -ForegroundColor Cyan
    } else {
        Write-Host "  ✗ Failed to generate $outputJS" -ForegroundColor Red
    }
} else {
    Write-Host "  ✗ Failed to compile - check your .algo file syntax" -ForegroundColor Red
}
```

**Usage:**
```powershell
.\run_complete.ps1                    # Uses test.algo → visualizer.js
.\run_complete.ps1 myfile.algo        # Uses myfile.algo → visualizer.js  
.\run_complete.ps1 test.algo my.js    # Uses test.algo → my.js
```

---

## ✅ What You Should See

**After Step 3a (`dsa_compiler.exe`):**
```
Visualization JSON generated successfully!
```

**After Step 3b (`json_to_js.exe`):**
```
JavaScript visualization code generated: visualizer.js
```

**If successful:**
- `output.json` exists with structured JSON
- `visualizer.js` exists with complete JavaScript code

---

## 🎬 Real Example

Let's say you have this in `test.algo`:

```
for (i = 0; i < n; i++) {
  if (arr[i] > arr[j]) {
    swap(arr[i], arr[j]);
  }
}
```

**Run:**
```powershell
.\dsa_compiler.exe test.algo
.\json_to_js.exe output.json visualizer.js
```

**Result:** You'll get a `visualizer.js` file that you can paste into Algorithm Visualizer to see a beautiful step-by-step animation of your algorithm!

---

## 💡 Tips

1. **First time?** Make sure both `.exe` files exist before running
2. **Changed source code?** Recompile both executables
3. **Syntax errors?** Check your `.algo` file format matches the grammar
4. **Want custom array?** Modify the default array in `codegen.c` or extend the compiler
5. **Multiple files?** Run the pipeline for each `.algo` file separately

---

**That's all! Just 2 commands to go from `.algo` to visualizable JavaScript!** 🚀

