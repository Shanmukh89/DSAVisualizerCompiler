# Complete Guide - All 5 Sorting Algorithms

## ✅ All Algorithms Working!

Your compiler now supports **5 sorting algorithms**:

1. **Bubble Sort** - `bubble_sort(arr)`
2. **Insertion Sort** - `insertion_sort(arr)`
3. **Selection Sort** - `selection_sort(arr)`
4. **Merge Sort** - `merge_sort(arr)`
5. **Quick Sort** - `quick_sort(arr)` or `quicksort(arr)`

---

## 🚀 How to Run Any Algorithm

### Step 1: Write Your DSL

Create a `.algo` file with array and algorithm:

**Example: `my_sort.algo`**
```
arr[]=[10,5,8,3,1];
insertion_sort(arr);
```

### Step 2: Compile

```powershell
.\dsa_compiler.exe my_sort.algo
```

### Step 3: Convert to JavaScript

```powershell
.\json_to_js.exe output.json visualizer.js
```

### Step 4: Visualize

1. Copy `visualizer.js` contents
2. Paste into Algorithm Visualizer
3. Click Run!

---

## 📋 Algorithm Examples

### Bubble Sort
```
arr[]=[5,3,8,4,2];
bubble_sort(arr);
```

### Insertion Sort
```
arr[]=[5,2,8,1,9];
insertion_sort(arr);
```

### Selection Sort
```
arr[]=[64,25,12,22,11];
selection_sort(arr);
```

### Merge Sort
```
arr[]=[38,27,43,3,9,82,10];
merge_sort(arr);
```

### Quick Sort
```
arr[]=[10,7,8,9,1,5];
quick_sort(arr);
```

---

## 🎯 Quick Test All Algorithms

```powershell
# Test each algorithm
.\dsa_compiler.exe insertion_sort_test.algo
.\json_to_js.exe output.json insertion.js

.\dsa_compiler.exe selection_sort_test.algo
.\json_to_js.exe output.json selection.js

.\dsa_compiler.exe merge_sort_test.algo
.\json_to_js.exe output.json merge.js

.\dsa_compiler.exe quick_sort_test.algo
.\json_to_js.exe output.json quick.js
```

---

## ✨ What Each Algorithm Generates

All algorithms generate:
- ✅ Proper compare steps with indices
- ✅ Swap/no_swap actions
- ✅ Mark sorted positions
- ✅ Complete visualization steps
- ✅ Ready-to-use JavaScript

---

## 📂 Test Files Created

- `test_new_dsl.algo` - Bubble Sort
- `insertion_sort_test.algo` - Insertion Sort
- `selection_sort_test.algo` - Selection Sort
- `merge_sort_test.algo` - Merge Sort
- `quick_sort_test.algo` - Quick Sort

---

## 🎉 Complete!

**All 5 sorting algorithms are fully implemented and working!**

You can now visualize any of these algorithms with your own arrays!

