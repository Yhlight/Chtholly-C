# Chtholly Development Roadmap

## Phase 1: Foundational Infrastructure

- [x] **Project Setup**: Initialize the project structure, including `src`, `tests`, and `scripts` directories.
- [x] **Build System**: Implement a CMake build system and a Python build script.
- [x] **Lexer**: Implement a lexer to tokenize the Chtholly source code.
- [x] **Parser**: Implement a recursive descent parser to build an Abstract Syntax Tree (AST).
- [x] **AST**: Define the data structures for the AST.
- [ ] **Basic Transpiler**: Implement a basic transpiler that can generate C++ code from a simple AST.

## Phase 2: Core Language Features

- [ ] **Variables**: Implement `let` and `mut` variable declarations.
- [ ] **Data Types**: Implement basic data types (int, double, bool, string).
- [ ] **Operators**: Implement basic arithmetic and logical operators.
- [ ] **Control Flow**: Implement `if`, `else`, and `while` statements.
- [ ] **Functions**: Implement function definitions and calls.

## Phase 3: Advanced Language Features

- [ ] **Structs**: Implement struct definitions and instantiation.
- [ ] **Ownership System**: Implement the ownership system with `&` and `&mut` references.
- [ ] **Generics**: Implement generic functions and structs.
- [ ] **Traits**: Implement traits for defining shared behavior.
- [ ] **Modules**: Implement the module system with `import`.

## Phase 4: Standard Library

- [ ] **iostream**: Implement the `iostream` module for input and output.
- [ ] **filesystem**: Implement the `filesystem` module for file system operations.
- [ ] **operator**: Implement the `operator` module for operator overloading.
- [ ] **reflect**: Implement the `reflect` module for static reflection.
- [ ] **meta**: Implement the `meta` module for metaprogramming.

## Phase 5: Self-Hosting

- [ ] **Self-Hosting Compiler**: Rewrite the Chtholly compiler in Chtholly.
