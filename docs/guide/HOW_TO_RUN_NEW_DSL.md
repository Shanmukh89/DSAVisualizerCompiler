# How to Run with New DSL Syntax

## ✅ Quick Start

Your compiler now supports the new DSL format! Here's how to use it:

### 1. Write Your DSL File

Create a `.algo` file with the new syntax:

**Example: `my_algorithm.algo`**
```
arr[]=[2,4,12,3,4];
bubble_sort(arr);
```

### 2. Run the Complete Pipeline

```powershell
# Step 1: Compile DSL to JSON
.\dsa_compiler.exe my_algorithm.algo

# Step 2: Convert JSON to JavaScript
.\json_to_js.exe output.json visualizer.js

# Step 3: Copy visualizer.js to Algorithm Visualizer
```

## 📝 Complete Example

### Your DSL File (`test_new_dsl.algo`):
```
arr[]=[2,4,12,3,4];
bubble_sort(arr);
```

### Run It:
```powershell
cd "C:\Users\shanm\Downloads\Academics\5th Sem\Compiler Design\Cursor\DSAVisualizerCompiler"

# Compile
.\dsa_compiler.exe test_new_dsl.algo

# Convert to JS
.\json_to_js.exe output.json visualizer.js

# View results
Get-Content output.json
Get-Content visualizer.js
```

### Output:

**`output.json`** will contain:
- Your array: `[2, 4, 12, 3, 4]`
- Algorithm: `"bubble_sort"`
- All visualization steps

**`visualizer.js`** will be ready to paste into Algorithm Visualizer!

## 🎯 What Happens

1. **Parser** reads your DSL:
   - `arr[]=[2,4,12,3,4]` → Extracts array values
   - `bubble_sort(arr)` → Detects algorithm name

2. **Codegen** generates steps:
   - Uses your array: `[2, 4, 12, 3, 4]`
   - Generates bubble sort steps with your array
   - Creates all passes and comparisons

3. **Translator** creates JavaScript:
   - Algorithm Visualizer compatible code
   - Ready to paste and visualize!

## 🔄 Full Workflow

```
my_algorithm.algo
    ↓
[dsa_compiler.exe]
    ↓
output.json (with your array and steps)
    ↓
[json_to_js.exe]
    ↓
visualizer.js (ready for Algorithm Visualizer!)
    ↓
[Copy & Paste into Algorithm Visualizer]
    ↓
✨ Beautiful Visualization!
```

## 💡 Tips

1. **Array values**: Use any numbers you want: `arr[]=[5,1,9,2,7];`
2. **Algorithm**: Currently supports `bubble_sort(arr)`
3. **File location**: Create `.algo` files in `DSAVisualizerCompiler` directory
4. **Naming**: Use descriptive names like `bubble_sort_test.algo`

## 📋 Example DSL Files

### Bubble Sort Test
**File:** `bubble_test.algo`
```
arr[]=[10,5,8,3,1];
bubble_sort(arr);
```

### Different Array
**File:** `test2.algo`
```
arr[]=[20,15,30,5,25];
bubble_sort(arr);
```

## ✨ That's It!

Just write your DSL, run the compiler, and visualize! 🚀

