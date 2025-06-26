# compiler

Custom compiler for a Python-like language, written in modern C++.

---

## 📌 Description

This project is a complete compiler pipeline being built from scratch in C++. The language being compiled is inspired by Python and dynamically typed, focusing on **fast computation** with **runtime type inference**, allowing types to change during execution (e.g., strings becoming integers when needed).

The compiler will ultimately support:

* **Lexical analysis (Tokenizer)**
* **Parsing (Recursive Descent Parser)**
* **AST Generation**
* **Dynamic Runtime Evaluation**
* (Later) Semantic checks, IR generation, optimization, and optional bytecode or C output.

---

## ✅ Work Done (as of June 26, 2025)

### 🔤 1. **Tokenizer (Lexical Analyzer)**

Fully implemented tokenizer that converts source code into a vector of tokens with positional metadata.

* ✅ Recognizes:

  * **Keywords**: `def`, `input`, `output`, `if`, `else`, `elif`, `for`, `return`, `true`, `false`, `exit`
  * **Operators**: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `&&`, `||`, `=`, etc.
  * **Separators**: `(`, `)`, `{`, `}`, `[`, `]`, `,`, `;`
  * **Literals**:

    * `int`, `float`, `bool`, `char`, `string`
  * **Identifiers**: Valid variable/function names (e.g., `a`, `x_12`, `main`)
* ✅ Robust error handling:

  * Malformed numbers
  * Unclosed strings/chars
  * Unknown characters
* ✅ Handles:

  * **Negative numbers**
  * **Single-line and multi-line comments**
* ✅ Outputs tokens for inspection with:

  ```cpp
  Token at 17: IDENTIFIER - x
  ```

---

### 📐 2. **Parser (Recursive Descent)**

Handwritten **recursive descent parser** that verifies the grammar of the language.

* ✅ Supports parsing:

  * Function definitions with parameters
  * Blocks `{ ... }`
  * Statements:

    * `input x;`
    * `output x + 5;`
    * `if (...) { ... } else { ... }`
    * `for (i=0; i<5; i=i+1) { ... }`
    * `return;`, `return x + 1;`
    * Assignments: `x = expr;`
  * Expressions:

    * Full precedence hierarchy: `||` → `&&` → `==`/`!=` → comparisons → `+`/`-` → `*`/`/`/`%` → unary → primary
    * Literals, identifiers, parenthesis, array access `arr[0]`, array literals `[1, 2]`, function calls `foo(1, 2)`
* ✅ Handles syntax errors gracefully with fallbacks
* ✅ Confirms structure is valid before moving to AST

---

### 🌲 3. **AST Generation**

AST (Abstract Syntax Tree) nodes are generated only after parsing succeeds.

* ✅ Implemented AST nodes for:

  * Expressions: literals, identifiers, binary/unary expressions, arrays, function calls
  * Statements: input/output, assignment, return, if/else, for loops
  * Structures: functions, blocks, entire program
* ✅ Pretty `print()` method for visualizing AST structure

---

### ⚙️ 4. **Runtime Value System** (Dynamic Typing)

Dynamic runtime value system to support automatic type inference at runtime.

* ✅ `RuntimeValue` class:

  * Holds `int`, `float`, `bool`, `string`, `array`, or `undefined`
  * Implements deep copy (copy constructor, assignment operator, destructor)
* ✅ Type coercion logic:

  * `"5" + 2` → becomes `7`
  * `"3.14" + 1` → becomes `4.14`
  * `"hello" + " world"` → string concatenation
  * `true + 1` → becomes `2`
* ✅ Binary operation handling:

  * Handles arithmetic, comparison, logical operations
  * Handles runtime errors like division/modulo by zero
* ✅ Unary operations:

  * `-x`, `!x`
* ✅ `toString()` and `print()` for value display

---

## 🧠 Language Behavior

The language behaves like **Python in type flexibility**, but aims to be **compiled for performance like C/C++**.

Examples of runtime type inference:

```python
input x         # x is a string
x = x + 5       # if x == "2", x becomes 7 (int)
x = x + "abc"   # becomes "7abc" (string)
x = 3.5         # now x is a float
```

---

## 🗂️ Directory Structure

```
compiler/
├── main.cpp                # Entry point – tokenizes, parses, builds AST
├── tokenizer.{h,cpp}       # Lexical analyzer
├── parser.{h,cpp}          # Recursive descent parser
├── ast.{h,cpp}             # AST structure
├── ast_generator.cpp       # AST generation from parser
├── runtime.{h,cpp}         # RuntimeValue and evaluator functions
└── README.md
```

---

## 🚧 Upcoming Tasks (July 2025)

* [ ] Build AST Evaluator: walk the AST and execute using `RuntimeValue`
* [ ] Add a variable environment (symbol table for runtime)
* [ ] Handle scope and function calling
* [ ] Add support for built-in functions (e.g., `len`, `print`)
* [ ] (Future) Code generation to C, IR, or bytecode

---
