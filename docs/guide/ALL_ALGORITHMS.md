# All Supported Sorting Algorithms

Your compiler now supports **5 sorting algorithms**! 🎉

## 📋 Supported Algorithms

### 1. Bubble Sort
```dsl
arr[]=[5,3,8,4,2];
bubble_sort(arr);
```

### 2. Insertion Sort
```dsl
arr[]=[5,2,8,1,9];
insertion_sort(arr);
```

### 3. Selection Sort
```dsl
arr[]=[64,25,12,22,11];
selection_sort(arr);
```

### 4. Merge Sort
```dsl
arr[]=[38,27,43,3,9,82,10];
merge_sort(arr);
```

### 5. Quick Sort
```dsl
arr[]=[10,7,8,9,1,5];
quick_sort(arr);
```
**Note:** Also accepts `quicksort(arr)` (without underscore)

---

## 🚀 How to Use

### Step 1: Create Your DSL File

Create a `.algo` file with array and algorithm call:

**Example: `my_sort.algo`**
```
arr[]=[10,5,8,2,7];
insertion_sort(arr);
```

### Step 2: Compile and Convert

```powershell
# Compile to JSON
.\dsa_compiler.exe my_sort.algo

# Convert to JavaScript
.\json_to_js.exe output.json visualizer.js
```

### Step 3: Visualize

1. Copy contents of `visualizer.js`
2. Paste into Algorithm Visualizer
3. Click Run!

---

## 📝 Example Test Files

Test files have been created for each algorithm:

- `test_new_dsl.algo` - Bubble Sort example
- `insertion_sort_test.algo` - Insertion Sort
- `selection_sort_test.algo` - Selection Sort
- `merge_sort_test.algo` - Merge Sort
- `quick_sort_test.algo` - Quick Sort

---

## 🎯 Quick Test All Algorithms

```powershell
# Test Bubble Sort
.\dsa_compiler.exe test_new_dsl.algo
.\json_to_js.exe output.json bubble.js

# Test Insertion Sort
.\dsa_compiler.exe insertion_sort_test.algo
.\json_to_js.exe output.json insertion.js

# Test Selection Sort
.\dsa_compiler.exe selection_sort_test.algo
.\json_to_js.exe output.json selection.js

# Test Merge Sort
.\dsa_compiler.exe merge_sort_test.algo
.\json_to_js.exe output.json merge.js

# Test Quick Sort
.\dsa_compiler.exe quick_sort_test.algo
.\json_to_js.exe output.json quick.js
```

---

## ✨ Features

Each algorithm generates:
- ✅ Proper visualization steps
- ✅ Compare actions with indices
- ✅ Swap/no_swap actions
- ✅ Mark sorted positions
- ✅ Algorithm-specific step patterns
- ✅ Ready-to-use JavaScript for Algorithm Visualizer

---

**All 5 sorting algorithms are now fully supported!** 🚀

