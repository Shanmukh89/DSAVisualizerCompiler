# CLI Visualizer Guide

The compiler now supports **two visualization modes**:

1. **CLI Visualization** (default) - Step-by-step visualization in terminal
2. **JavaScript Generation** - For Algorithm Visualizer web interface

## Usage

### CLI Visualization (Default)

```bash
.\dsa_compiler.exe test.algo
# or explicitly:
.\dsa_compiler.exe -v test.algo
```

This will:
1. Compile the DSL file
2. Generate JSON IR
3. Launch interactive CLI visualization
4. Show each step with arrows, boxes, and colored output

### JavaScript Generation (For Web)

```bash
.\dsa_compiler.exe -j test.algo
# or with custom output:
.\dsa_compiler.exe -j test.algo -o visualizer.js
```

This will:
1. Compile the DSL file
2. Generate JSON IR
3. Convert to JavaScript
4. Create a file ready to paste into Algorithm Visualizer

## CLI Visualization Features

### Visual Elements

1. **Array Display**:
   ```
   ┌───────┬───────┬───────┬───────┐
   │  [ 2]  │  [ 4]  │  [12]  │  [ 3]  │
   └───────┴───────┴───────┴───────┘
    ( 0)   ( 1)   ( 2)   ( 3)
   ```

2. **Color Coding**:
   - 🔴 **Red** - Elements being compared/swapped
   - 🟢 **Green** - Sorted elements (in final position)
   - 🔵 **Cyan** - Array borders
   - 🟡 **Yellow** - Action descriptions

3. **Step Descriptions**:
   - `→ Comparing: 4 and 12`
   - `  4 ≤ 12 → No swap needed`
   - `⇄ Swapping: 12 and 3`
   - `  Since 12 > 3`

### Interactive Navigation

- Press **any key** to advance to next step
- Press **Ctrl+C** to exit visualization

### Supported Actions

- **compare** - Shows comparison between two elements
- **swap** - Shows swapping with explanation
- **no_swap** - Shows no swap needed
- **mark_sorted** - Highlights sorted element
- **push/pop** - Stack operations
- **enqueue/dequeue** - Queue operations

## Examples

### Example 1: Bubble Sort Visualization

```algo
arr[]=[5,3,8,2,1];
bubble_sort(arr);
```

**CLI Output:**
```
═══════════════════════════════════════════════════════════
  Bubble Sort Visualization
═══════════════════════════════════════════════════════════

→ Comparing: 5 and 3
  5 > 3 → Swap needed!

┌───────┬───────┬───────┬───────┬───────┐
│  [ 5]  │  [ 3]  │  [ 8]  │  [ 2]  │  [ 1]  │
└───────┴───────┴───────┴───────┴───────┘
  ( 0)   ( 1)   ( 2)   ( 3)   ( 4)

Press any key to continue...
```

### Example 2: Generate JavaScript

```bash
.\dsa_compiler.exe -j test.algo
```

**Output:**
```
Compiling: test.algo
Phase 1: Lexical Analysis...
Phase 2: Syntax Analysis...
Phase 3: Semantic Analysis...
Phase 4: Code Generation...

Converting to JavaScript...

✓ JavaScript code generated: visualizer.js
  Paste this into Algorithm Visualizer: https://algorithm-visualizer.org/
```

## Command Line Options

```
Usage: dsa_compiler.exe [OPTIONS] <input_file>

Options:
  -h, --help          Show help message
  -v, --visualize     Visualize in terminal (default)
  -j, --json-to-js    Convert to JavaScript for web
  -o, --output FILE   Specify output file

Examples:
  dsa_compiler.exe test.algo              # CLI visualization
  dsa_compiler.exe -v test.algo           # Same as above
  dsa_compiler.exe -j test.algo           # Generate JavaScript
  dsa_compiler.exe -j test.algo -o viz.js # Custom output file
```

## Workflow Comparison

### CLI Visualization Workflow
```
DSL File → Compiler → JSON → CLI Visualizer → Terminal Output
```

### JavaScript Workflow
```
DSL File → Compiler → JSON → JS Converter → visualizer.js → Algorithm Visualizer
```

## Technical Details

### CLI Visualizer (`cli_visualizer.exe`)

- Reads JSON IR file
- Parses steps sequentially
- Displays animated visualization
- Updates array state in real-time
- Supports all algorithm types

### JavaScript Generator (`json_to_js.exe`)

- Reads JSON IR file
- Generates Algorithm Visualizer compatible code
- Includes all tracer API calls
- Ready for web visualization

## Tips

1. **For Learning**: Use CLI visualization to understand step-by-step execution
2. **For Presentation**: Use JavaScript mode for web-based demos
3. **For Debugging**: CLI mode shows detailed state at each step
4. **For Sharing**: JavaScript mode creates shareable visualization code

## Troubleshooting

### CLI Visualizer not starting
- Check that `output.json` exists
- Ensure compilation was successful
- Try running `.\cli_visualizer.exe output.json` directly

### Colors not showing
- Windows 10+ supports ANSI colors
- Older Windows may show escape codes instead
- Use a modern terminal (Windows Terminal, PowerShell 7+)

### JavaScript not generating
- Ensure `json_to_js.exe` exists
- Check that JSON file is valid
- Verify output path is writable

