# Error Handling and Validation

The compiler now includes comprehensive error detection and validation, behaving like a real compiler that reports errors instead of silently producing incorrect output.

## Validation Features

### 1. Algorithm Name Validation

The compiler validates algorithm names and reports errors for invalid algorithms.

**Valid Algorithms:**
- `bubble_sort`
- `insertion_sort`
- `selection_sort`
- `merge_sort`
- `quick_sort` or `quicksort`

**Example Error:**
```bash
$ .\dsa_compiler.exe test_error.algo
Error: Unknown algorithm: 'bule_sort'. Valid algorithms are: bubble_sort, insertion_sort, selection_sort, merge_sort, quick_sort
Did you mean: bubble_sort?
Compilation failed due to errors.
```

### 2. Data Structure Type Validation

Invalid data structure names are detected.

**Valid Data Structures:**
- `stack`
- `queue`
- `tree`
- `graph`

**Example:**
```
stak.push(10);  // Error: Unknown data structure
```

### 3. Operation Validation

Operations are validated per data structure type.

#### Stack Operations
- Valid: `push`, `pop`, `peek`, `top`
- Invalid: `pus`, `add`, `remove`, etc.

**Example Error:**
```bash
$ .\dsa_compiler.exe test_error3.algo
Error: Invalid stack operation: 'pus'. Valid operations are: push, pop, peek, top
Compilation failed due to errors.
```

#### Queue Operations
- Valid: `enqueue`, `dequeue`

#### Tree Operations
- Valid: `insert`, `traverse_inorder`, `traverse_preorder`, `traverse_postorder`

#### Graph Operations
- Valid: `add_node`, `add_edge`, `bfs`, `dfs`

## Error Messages

All errors include:
1. **Error Type**: What went wrong
2. **Invalid Value**: What was provided
3. **Valid Options**: What is allowed
4. **Helpful Suggestions**: When applicable (e.g., "Did you mean: bubble_sort?")

## Exit Codes

- **Exit Code 0**: Compilation successful
- **Exit Code 1**: Compilation failed (parse error, validation error, or generation error)

## Behavior Changes

### Before
- Invalid algorithm names were silently accepted
- Default fallback to bubble_sort was used
- No error reporting

### After
- Invalid names are detected and reported
- Compilation fails with clear error messages
- No output file generated on error
- Valid names work as expected

## Example Test Cases

### Valid Input ✅
```algo
arr[]=[2,4,12,3,4];
bubble_sort(arr);
```
```
Visualization JSON generated successfully!
```

### Invalid Algorithm ❌
```algo
arr[]=[2,4,12,3,4];
bule_sort(arr);
```
```
Error: Unknown algorithm: 'bule_sort'. Valid algorithms are: bubble_sort, insertion_sort, selection_sort, merge_sort, quick_sort
Did you mean: bubble_sort?
Compilation failed due to errors.
```

### Invalid Operation ❌
```algo
stack.pus(10);
stack.push(20);
```
```
Error: Invalid stack operation: 'pus'. Valid operations are: push, pop, peek, top
Compilation failed due to errors.
```

## Implementation Details

Validation happens in the code generation phase:
1. **Collection Phase**: Algorithm/data structure names are collected
2. **Validation Phase**: Names are checked against valid lists
3. **Error Reporting**: Invalid names trigger error messages
4. **Early Exit**: Generation stops on first error

## Future Enhancements

Potential improvements:
- Typo detection with suggestions (e.g., "Did you mean 'bubble_sort'?")
- Multiple error collection (report all errors, not just first)
- Line number information in error messages
- Warning system for deprecated or uncommon patterns

