# DSL Writing Guide - Where to Write Your Algorithms

## 📝 Where to Write DSL Code

### Option 1: Main Project Directory (Recommended)

Create `.algo` files directly in the `DSAVisualizerCompiler` directory:

```
DSAVisualizerCompiler/
├── test.algo          ← Main test file (already exists)
├── bubble_sort.algo   ← Create your own files here
├── quicksort.algo     ← Any name with .algo extension
├── insertion_sort.algo
└── ...
```

**Example: Creating a new file**

1. Create a new file: `my_algorithm.algo`
2. Write your DSL code in it
3. Run: `.\dsa_compiler.exe my_algorithm.algo`

### Option 2: Test Inputs Directory

You can also create files in `tests/inputs/` directory:

```
DSAVisualizerCompiler/
└── tests/
    └── inputs/
        ├── case1_assign.algo      ← Existing test files
        ├── case2_if_call.algo
        ├── case3_for_if_call.algo
        └── my_new_test.algo       ← Create new ones here
```

**Example: Using test directory**

```powershell
# Create file in tests/inputs/
New-Item -Path "tests\inputs\my_test.algo" -ItemType File

# Run it
.\dsa_compiler.exe tests\inputs\my_test.algo
```

---

## 📋 DSL Syntax Reference

### Valid DSL Constructs

#### 1. **Assignments**
```
x = 5;
arr[i] = 10;
```

#### 2. **For Loops**
```
for (i = 0; i < n; i++) {
  // statements
}
```

#### 3. **If Statements**
```
if (arr[i] > arr[j]) {
  swap(arr[i], arr[j]);
}
```

#### 4. **Function Calls**
```
swap(arr[i], arr[j]);
```

---

## 🎯 Quick Examples

### Example 1: Simple Assignment
**File:** `simple.algo`
```
x = 1;
y = 2;
```

### Example 2: Bubble Sort
**File:** `bubble_sort.algo`
```
for (i = 0; i < n; i++) {
  if (arr[i] > arr[j]) {
    swap(arr[i], arr[j]);
  }
}
```

### Example 3: Multiple Loops
**File:** `nested.algo`
```
for (i = 0; i < n; i++) {
  for (j = 0; j < m; j++) {
    if (arr[i] > arr[j]) {
      swap(arr[i], arr[j]);
    }
  }
}
```

---

## 🚀 How to Use

### Step 1: Create Your DSL File

**In PowerShell:**
```powershell
cd "C:\Users\shanm\Downloads\Academics\5th Sem\Compiler Design\Cursor\DSAVisualizerCompiler"

# Create a new file
notepad my_algorithm.algo
# Or use any text editor (VS Code, Notepad++, etc.)
```

**Or manually:**
- Create a new file in the `DSAVisualizerCompiler` folder
- Name it anything with `.algo` extension (e.g., `bubble_sort.algo`)
- Write your algorithm code

### Step 2: Write Your Algorithm

Edit your `.algo` file with DSL code:

```
for (i = 0; i < n; i++) {
  if (arr[i] > arr[j]) {
    swap(arr[i], arr[j]);
  }
}
```

### Step 3: Compile It

```powershell
# Compile your algorithm
.\dsa_compiler.exe my_algorithm.algo

# This generates: output.json
```

### Step 4: Convert to JavaScript

```powershell
# Convert to JavaScript
.\json_to_js.exe output.json visualizer.js

# This generates: visualizer.js
```

### Step 5: Visualize

1. Open https://algorithm-visualizer.org/
2. Copy contents of `visualizer.js`
3. Paste into editor
4. Click Run!

---

## 📂 Current Files

**Main directory files:**
- `test.algo` - Your main test file (currently open)

**Test input files:**
- `tests/inputs/case1_assign.algo` - Simple assignment
- `tests/inputs/case2_if_call.algo` - If statement with function call
- `tests/inputs/case3_for_if_call.algo` - Loop with if and function call

---

## 💡 Tips

1. **File naming:** Use descriptive names like `bubble_sort.algo`, `my_test.algo`
2. **Location:** Keep files in the main `DSAVisualizerCompiler` directory for easy access
3. **Syntax:** Follow the DSL grammar (for loops, if statements, assignments)
4. **Testing:** Test with small algorithms first
5. **Multiple files:** You can create as many `.algo` files as you want!

---

## 🔍 Quick Start Example

```powershell
# 1. Navigate to directory
cd "C:\Users\shanm\Downloads\Academics\5th Sem\Compiler Design\Cursor\DSAVisualizerCompiler"

# 2. Create/edit your DSL file
notepad my_bubble_sort.algo

# 3. Write this in the file:
# for (i = 0; i < n; i++) {
#   if (arr[i] > arr[j]) {
#     swap(arr[i], arr[j]);
#   }
# }

# 4. Save and run
.\dsa_compiler.exe my_bubble_sort.algo
.\json_to_js.exe output.json my_visualization.js

# 5. View result
Get-Content my_visualization.js
```

---

**That's it!** Write your DSL code in any `.algo` file in the `DSAVisualizerCompiler` directory! 🎉

