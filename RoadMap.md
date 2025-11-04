# Chtholly Development Roadmap

## Phase 1: Foundational Infrastructure

- [x] **Project Setup**: Initialize the project structure, including `src`, `tests`, and `scripts` directories.
- [x] **Build System**: Implement a CMake build system and a Python build script.
- [x] **Lexer**: Implement a lexer to tokenize the Chtholly source code.
- [x] **Parser**: Implement a parser to build an Abstract Syntax Tree (AST). (Note: Expression parsing has been refatored to use a Pratt parser for better operator-precedence handling.)
- [x] **AST**: Define the data structures for the AST.
- [x] **Basic Transpiler**: Implement a basic transpiler that can generate C++ code from a simple AST.

## Phase 2: Core Language Features

- [x] **Variables**: Implement `let` and `mut` variable declarations.
- [x] **Data Types**: Implement basic data types (int, double, bool, string).
- [x] **Operators**: Implement basic arithmetic and logical operators.
- [x] **Control Flow**: Implement `if`, `else`, `while`, and `switch` statements.
- [x] **Functions**: Implement function definitions and calls.

## Phase 3: Advanced Language Features

- [x] **Structs**: Implement struct definitions, instantiation, and field access.
- [x] **Access Modifiers**: Implement `public` and `private` access control for struct fields.
- [x] **Ownership System**: Implement the ownership system with `&` and `&mut` references.
- [x] **Generics**: Implement generic functions and structs.
- [x] **Lambda Expressions and Function Types**: Implement lambda expressions and function types.
- [x] **Traits**: Implement traits for defining shared behavior and operator overloading.
- [x] **Modules**: Implement the module system with `import`.

## Phase 4: Standard Library

- [x] **iostream**: Implement the `iostream` module for input and output.
- [x] **filesystem**: Implement the `filesystem` module for file system operations.
- [x] **operator**: Implement the `operator` module for operator overloading.
- [~] **reflect**: Implement the `reflect` module for static reflection. (Note: Static field name reflection has been implemented.)
- [~] **meta**: Implement the `meta` module for metaprogramming. (Note: Type-checking functions like `is_int`, `is_struct`, etc., have been implemented.)

## Phase 5: Compiler Internals Refactoring

- [ ] **Error Reporting**: Refactor the `ErrorReporter` to provide more detailed contextual information, such as line and column numbers, and potentially code snippets. Implement a more robust panic-mode recovery in the parser.
- [ ] **Resolver Decomposition**: Break down the monolithic `Resolver` class into smaller, more focused components.
    - [ ] Create a dedicated `ScopeManager` to handle lexical scopes.
    - [ ] Implement a `TypeChecker` to manage type inference and validation.
    - [ ] Develop a `BorrowChecker` to handle ownership and borrowing rules explicitly.
- [ ] **Transpiler Extensibility**: Refactor the `Transpiler` to be more data-driven and extensible, making it easier to add support for new language features and backend targets in the future.
- [ ] **AST Semantics**: Enhance the AST nodes to store more semantic information post-resolution (e.g., resolved types, symbol references), reducing the `Transpiler`'s dependency on the `Resolver`.
- [ ] **Performance Profiling**: Profile the compiler to identify and optimize key performance bottlenecks in the lexing, parsing, and transpilation stages.

## Phase 6: Codebase Structure Modernization

- [ ] **Directory Restructuring**: Reorganize the `src` directory to better reflect the compiler's architecture.
    - [ ] `src/frontend`: For components that process the source language (Lexer, Parser, AST).
    - [ ] `src/middle`: For semantic analysis (Resolver and its sub-components).
    - [ ] `src/backend`: For the C++ code generation (Transpiler).
    - [ ] `src/driver`: For the main compiler orchestration logic (`Chtholly` class) and file handling.
- [ ] **Build System Update**: Update `CMakeLists.txt` to reflect the new directory structure and create distinct static libraries for each major component (e.g., `chtholly_frontend`, `chtholly_middle`).
- [ ] **Component Migration**: Gradually migrate existing components into the new structure, starting with the most independent ones (e.g., AST, Lexer).
- [ ] **Test Suite Expansion**: Expand the test suite to cover each component in isolation, facilitating more targeted and effective testing.

**Note on Project Direction**: The original self-hosting plan has been retired. The project's focus has shifted to creating a single, robust, and maintainable C++ compiler for the Chtholly language. These refactoring phases are the first steps in that direction.
