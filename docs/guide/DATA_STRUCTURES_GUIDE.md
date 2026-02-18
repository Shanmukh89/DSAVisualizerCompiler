# Data Structures DSL Guide

This guide explains how to use the compiler's DSL to visualize various data structures and their operations.

## Supported Data Structures

### 1. Stack Operations

Stack follows LIFO (Last In First Out) principle.

**Syntax:**
```
stack.push(value);
stack.pop();
stack.peek();  // or stack.top();
```

**Example:**
```
stack.push(10);
stack.push(20);
stack.push(30);
stack.peek();
stack.pop();
stack.pop();
```

**Operations:**
- `push(value)`: Push an element onto the stack
- `pop()`: Remove and return the top element
- `peek()` or `top()`: View the top element without removing it

---

### 2. Queue Operations

Queue follows FIFO (First In First Out) principle.

**Syntax:**
```
queue.enqueue(value);
queue.dequeue();
```

**Example:**
```
queue.enqueue(10);
queue.enqueue(20);
queue.enqueue(30);
queue.dequeue();
queue.dequeue();
```

**Operations:**
- `enqueue(value)`: Add an element to the rear of the queue
- `dequeue()`: Remove and return the front element

---

### 3. Binary Tree Operations

Binary tree supports insertion and traversal operations.

**Syntax:**
```
tree.insert(value);
tree.traverse_inorder();
tree.traverse_preorder();
tree.traverse_postorder();
```

**Example:**
```
tree.insert(5);
tree.insert(3);
tree.insert(7);
tree.insert(1);
tree.insert(4);
tree.traverse_inorder();
```

**Operations:**
- `insert(value)`: Insert a node with the given value
- `traverse_inorder()`: Perform inorder traversal (Left, Root, Right)
- `traverse_preorder()`: Perform preorder traversal (Root, Left, Right)
- `traverse_postorder()`: Perform postorder traversal (Left, Right, Root)

---

### 4. Graph Operations

Graph supports node/edge creation and traversal algorithms.

**Syntax:**
```
graph.add_node(node_id);
graph.add_edge(from_node, to_node);
graph.bfs(start_node);    // Breadth-First Search
graph.dfs(start_node);    // Depth-First Search
```

**Example:**
```
graph.add_node(1);
graph.add_node(2);
graph.add_node(3);
graph.add_node(4);
graph.add_edge(1, 2);
graph.add_edge(1, 3);
graph.add_edge(2, 4);
graph.bfs(1);
```

**Operations:**
- `add_node(node_id)`: Add a node to the graph
- `add_edge(from, to)`: Add an edge between two nodes (undirected)
- `bfs(start)`: Perform Breadth-First Search starting from the given node
- `dfs(start)`: Perform Depth-First Search starting from the given node

---

## Running Data Structure Visualizations

### Step 1: Write your DSL file

Create a file (e.g., `test_stack.algo`) with your data structure operations:

```algo
stack.push(10);
stack.push(20);
stack.push(30);
stack.pop();
```

### Step 2: Compile to JSON IR

```bash
.\dsa_compiler.exe test_stack.algo
```

This generates `output.json` with the visualization steps.

### Step 3: Convert to JavaScript

```bash
.\json_to_js.exe output.json visualizer.js
```

### Step 4: Visualize

Copy the contents of `visualizer.js` and paste into [Algorithm Visualizer](https://algorithm-visualizer.org/) to see the visualization.

---

## Examples

### Stack Example
File: `test_stack.algo`
```
stack.push(10);
stack.push(20);
stack.push(30);
stack.peek();
stack.pop();
stack.pop();
```

### Queue Example
File: `test_queue.algo`
```
queue.enqueue(10);
queue.enqueue(20);
queue.enqueue(30);
queue.dequeue();
queue.dequeue();
```

### Tree Example
File: `test_tree.algo`
```
tree.insert(5);
tree.insert(3);
tree.insert(7);
tree.insert(1);
tree.insert(4);
tree.traverse_inorder();
```

### Graph Example
File: `test_graph.algo`
```
graph.add_node(1);
graph.add_node(2);
graph.add_node(3);
graph.add_node(4);
graph.add_edge(1, 2);
graph.add_edge(1, 3);
graph.add_edge(2, 4);
graph.bfs(1);
```

---

## Notes

1. **Method Call Syntax**: Data structure operations use the format `structure.operation(args)`
2. **Multiple Arguments**: Operations like `add_edge` take two arguments: `graph.add_edge(1, 2)`
3. **Order Matters**: Operations are executed in the order they appear in the file
4. **Visualization**: The generated JavaScript uses Algorithm Visualizer's tracer API

---

## Supported Visualizations

All data structures generate visualization code compatible with Algorithm Visualizer:
- **Stack/Queue**: Uses Array1DTracer with patch/depatch operations
- **Tree**: Uses node selection/deselection for traversal
- **Graph**: Uses node selection and edge tracking for BFS/DFS

---

## Troubleshooting

1. **"Unknown token: ." error**: Regenerate the lexer: `win_flex lexer.l` then recompile
2. **Syntax errors**: Ensure all statements end with semicolons
3. **Missing operations**: Check that operation names match exactly (case-sensitive)

---

## Future Extensions

Potential additions:
- Heap operations (min-heap, max-heap)
- Hash table operations
- Linked list operations
- Advanced tree operations (AVL, Red-Black trees)
- Graph algorithms (Dijkstra's, Prim's, Kruskal's)

