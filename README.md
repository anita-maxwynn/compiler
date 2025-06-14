
---

### 📄 `README.md` – Updated Version:

```markdown
# compiler

Custom compiler from scratch.

## Description

This project is a custom compiler built from the ground up, written in C++. It aims to tokenize, parse, and eventually compile a Python-like language. The long-term goal is to build all essential components including lexical analysis, parsing, semantic analysis, intermediate representation (IR), and code generation.

## 🛠️ Work Done Today (June 14, 2025)

- Initialized tokenizer with support for:
  - Keywords: `def`, `true`, `false`, `input`, `output`, `exit`
  - Literals: string, number, boolean
  - Separators: `;`, `()`, `{}`, `=`
- Built a basic lexical analyzer loop
- Printed identified tokens with their types
- Set up enums for `TokenTypeKeyword` and `TokenTypeSeparator`
- Cleaned up token classification logic for maintainability
- Verified output by running a test file with valid syntax

## 🗂️ Directory Structure

```

compiler/
├── main.cpp
├── tokenizer.cpp
├── tokenizer.h
└── README.md

```

## 🚧 Upcoming Tasks

- Add support for identifiers and operators
- Implement a symbol table
- Begin working on a recursive descent parser
```

---
