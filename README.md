# DSA Visualizer Compiler

**Lex/Yacc + C** — Compiles a small DSL for Data Structures and Algorithms into:

| Output | Description |
|--------|-------------|
| **JSON IR** | Step-by-step visualization data |
| **CLI visualizer** | Terminal playback (sorting + Stack, Queue, Tree, Graph) |
| **JavaScript** | Optional output for Algorithm Visualizer |

---

## Quick Start

### Build

Requires: `flex`, `bison`, `gcc`

```bash
make
```

**Windows:** From project root run `.\scripts\run.ps1`, or after generating parser/lexer:

```powershell
gcc -std=c11 -Wall -Wextra -Werror -Isrc -o dsa_compiler parser.tab.c lex.yy.c src/ast.c src/ir.c src/codegen.c src/main.c -lfl
gcc -std=c11 -Wall -Wextra -o json_to_js src/json_to_js.c
gcc -std=c11 -Wall -Wextra -o cli_visualizer.exe src/cli_visualizer.c
```

### Run

```bash
./dsa_compiler tests/examples/test.algo    # → output.json
./cli_visualizer output.json
./json_to_js output.json visualizer.js    # optional: JS for web visualizer
```

**Windows:**

```powershell
.\dsa_compiler.exe tests\examples\test.algo
.\cli_visualizer.exe output.json
```

---

## Features

- **Sorting:** Bubble, Insertion, Selection, Merge, Quick
- **Data structures:** Stack, Queue, BST (insert + traversals), Graph (nodes, edges, BFS/DFS)
- **Pipeline:** Lex → Parse (AST) → IR → JSON; CLI and optional JS output
- **Error handling:** Lexical, syntax, semantic with line info

---

## Repo Layout

| Path | Contents |
|------|----------|
| `src/` | `lexer.l`, `parser.y`, `ast.c/h`, `ir.c/h`, `codegen.c/h`, `main.c`, `cli_visualizer.c`, `json_to_js.c` |
| `docs/` | `LANGUAGE_REFERENCE.md`, `project_report.tex`, `guide/` (how-tos) |
| `tests/` | `inputs/`, `expected/` (tests), `examples/` (sample `.algo`) |
| `scripts/` | `run.ps1` (Windows build + test) |
| `web/` | `visualizer.js` |

---

## DSL Snippets

**Arrays & sorting**

```text
arr[] = [5,3,8,4,2];
bubble_sort(arr);   insertion_sort(arr);   merge_sort(arr);   quick_sort(arr);
```

**Stack / Queue**

```text
stack.push(10); stack.pop(); stack.peek();
queue.enqueue(10); queue.dequeue();
```

**BST**

```text
tree.insert(5); tree.insert(3);
tree.traverse_inorder(); tree.traverse_preorder(); tree.traverse_postorder();
```

**Graph**

```text
graph.add_node(1); graph.add_edge(1, 2); graph.bfs(1); graph.dfs(1);
```

> Full reference: `docs/guide/DATA_STRUCTURES_GUIDE.md`, `docs/LANGUAGE_REFERENCE.md`

---

## Pipeline

1. **Lex** (`lexer.l`) → tokens
2. **Parse** (`parser.y`) → AST (`ast.h`)
3. **Semantic** (`codegen.c`) → validate algorithms/DS ops
4. **IR** (`codegen.c` → `ir.c`) → step list → `output.json`
5. **Visualize:** CLI reads `output.json`; `json_to_js` produces JS for Algorithm Visualizer

---

## IR JSON (sketch)

```json
{
  "algorithm": "Bubble Sort | Stack Operations | ...",
  "array": [ ... ],
  "steps": [
    { "action": "compare", "indices": [0, 1] },
    { "action": "swap", "indices": [0, 1] },
    { "action": "push", "indices": [pos, val] },
    { "action": "insert", "indices": [value, parentValue, isLeft, meta] }
  ]
}
```

---

## CLI Visualizer

Reads `output.json` and shows:

| Type | Shown |
|------|--------|
| **Sorting** | Comparisons, swaps, passes, array state |
| **Stack** | Pushes/pops, top, size |
| **Queue** | Enqueue/dequeue, front, rear, size |
| **Tree** | Inserts, traversal visits, root, height |
| **Graph** | Nodes, edges, BFS/DFS |

DS views use DS-specific wording (no “sorted array” / “comparisons” for non-sorting).

---

## Extending

1. **New syntax:** Update `parser.y`, add tokens in `lexer.l` if needed.
2. **New IR steps:** In `codegen.c` use `addIR` / `addIRWithIndices`.
3. **CLI:** Handle new `action` types in `cli_visualizer.c`.
4. **JS:** Extend `json_to_js.c` for new tracer calls.

---

## Testing

- Add `.algo` files under `tests/inputs/`.
- Run the compiler and compare `output.json` to `tests/expected/<name>.json`.
- **Windows:** `.\scripts\run.ps1` to build and run tests.

---

## Troubleshooting

| Issue | Fix |
|-------|-----|
| `cli_visualizer.exe` not found | `gcc -std=c11 -Wall -Wextra -o cli_visualizer.exe src/cli_visualizer.c` then `.\cli_visualizer.exe output.json` from project root |
| `-lfl` link error (Windows) | Omit `-lfl`; `lexer.l` already defines `yywrap` |
| Bad colors/box-drawing | Use Windows 10+ or enable ANSI in terminal |

---

## License

Educational use.
