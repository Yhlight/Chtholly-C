# Chtholly Development Roadmap

## Phase 1: Foundational Infrastructure

- [x] **Project Setup**: Initialize the project structure, including `src`, `tests`, and `scripts` directories.
- [x] **Build System**: Implement a CMake build system and a Python build script.
- [x] **Lexer**: Implement a lexer to tokenize the Chtholly source code.
- [~] **Parser**: Implement a parser to build an Abstract Syntax Tree (AST). (Note: Expression parsing has been refactored to use a Pratt parser.)
- [x] **AST**: Define the data structures for the AST.
- [~] **Basic Transpiler**: Implement a basic transpiler that can generate C++ code from a simple AST.

## Phase 2: Core Language Features

- [x] **Variables**: Implement `let` and `mut` variable declarations.
- [x] **Data Types**: Implement basic data types (int, double, bool, string).
- [x] **Operators**: Implement basic arithmetic and logical operators.
- [x] **Control Flow**: Implement `if`, `else`, and `while` statements.
- [x] **Functions**: Implement function definitions and calls.

## Phase 3: Advanced Language Features

- [~] **Structs**: Implement struct definitions and instantiation. (Note: Struct initialization syntax is not fully implemented in the parser and resolver.)
- [x] **Ownership System**: Implement the ownership system with `&` and `&mut` references.
- [x] **Generics**: Implement generic functions and structs.
- [x] **Lambda Expressions and Function Types**: Implement lambda expressions and function types.
- [~] **Traits**: Implement traits for defining shared behavior. (Note: The resolver and transpiler have incomplete and partially incorrect implementations.)
- [~] **Modules**: Implement the module system with `import`. (Note: The resolver does not correctly import symbols from other modules.)

## Phase 4: Standard Library

- [x] **iostream**: Implement the `iostream` module for input and output.
- [~] **filesystem**: Implement the `filesystem` module for file system operations. (In Progress)
- [~] **operator**: Implement the `operator` module for operator overloading. (Note: The resolver and transpiler have incomplete and partially incorrect implementations.)
- [ ] **reflect**: Implement the `reflect` module for static reflection.
- [ ] **meta**: Implement the `meta` module for metaprogramming.

## Phase 5: Self-Hosting

- [ ] **Self-Hosting Compiler**: Rewrite the Chtholly compiler in Chtholly.

## Short-Term Plan: Stabilize Core Features

Based on a recent project review, the following tasks have been prioritized to stabilize the core language features and fix the currently failing tests.

1.  **Fix Struct Initialization**:
    *   Add a `StructInitializerExpr` to the AST.
    *   Implement parsing for struct initialization syntax (e.g., `Point{x: 1.0, y: 2.0}`).
    *   Implement semantic analysis for struct initializers in the `Resolver`.
    *   Implement C++ code generation for struct initializers in the `Transpiler`.

2.  **Fix Trait Method Resolution and Transpilation**:
    *   Update `Resolver::visitGetExpr` to correctly resolve method calls on objects that implement traits.
    *   Update `Transpiler` to correctly transpile calls to trait methods.

3.  **Fix Module System**:
    *   Update `Resolver::visitImportStmt` to correctly import symbols (functions, structs, etc.) from other modules into the current scope.
    *   Update `Transpiler::visitImportStmt` to ensure that the C++ code for an imported module is only generated once.
