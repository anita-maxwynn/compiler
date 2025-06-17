---

### 📄 `README.md` – Updated Version

# compiler

Custom compiler from scratch.

## Description

This project is a custom compiler built from the ground up using C++. It is designed to compile a Python-like language. The compiler will include components such as lexical analysis, parsing, semantic analysis, intermediate representation (IR), and code generation.

## 🛠️ Work Done (as of June 17, 2025)

* Implemented a robust tokenizer supporting:

  * **Keywords**: `def`, `true`, `false`, `input`, `output`, `exit`, `if`, `else`, `elif`, `for`, `return`
  * **Literals**: integer, float, boolean, string, character
  * **Operators**: arithmetic, logical, comparison, assignment, bitwise, shift
  * **Separators**: `;`, `()`, `{}`, `[]`, `,`
  * **Identifiers**: including valid names like `a_1`, `main`, etc.
* Added support for:

  * Negative numbers
  * Multi-line and single-line comments
  * Error handling for malformed tokens (e.g. unclosed char/string, bad numbers)
* Printed all tokens with positional info for debugging
* Structured enums and token structs for extensibility

## 🗂️ Directory Structure

```
compiler/
├── main.cpp
├── tokenizer.cpp
├── tokenizer.h
└── README.md
```

## 🚧 Upcoming Tasks

* Refactor and finalize tokenizer logic
* Start building the **recursive descent parser**
* Implement a symbol table for tracking identifiers
* Prepare for AST generation

---