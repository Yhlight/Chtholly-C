# Chtholly Development Roadmap

The self-hosting goal has been cancelled to focus on creating a robust and feature-rich C++ implementation for the open-source community.

## Phase 1: Stabilization and Core Refinement

- [ ] **Parser Overhaul**: Replace the recursive descent parser with a more robust Pratt parser to better handle operator precedence and complex expressions.
- [ ] **Semantic Analysis (Resolver) Hardening**: Improve the borrow checker, type checking, and error reporting. Implement more robust scope management, especially for modules.
- [ ] **Transpiler Improvements**: Refactor the transpiler for better C++ code generation, focusing on correctness, readability, and performance. Ensure correct type mapping.
- [ ] **Comprehensive Test Suite**: Augment the existing test suite to cover more edge cases and ensure stability during refactoring.

## Phase 2: Standard Library Completion

- [ ] **filesystem**: Finalize implementation for file system operations.
- [ ] **operator**: Implement operator overloading via traits (`Add`, `Sub`, etc.).
- [ ] **reflect**: Complete the static reflection capabilities.
- [ ] **meta**: Finalize compile-time metaprogramming features.
- [ ] **util**: Add a utility module for common tasks like string conversions.

## Phase 3: New Language Features

- [ ] **Enums and Pattern Matching**: Implement algebraic data types and a `match` statement for pattern matching.
- [ ] **Enhanced Control Flow**: Add `for` loops (including range-based loops).
- [ ] **Error Handling**: Introduce a robust error handling mechanism (e.g., `Result<T, E>` type and `?` operator).
- [ ] **Macros**: Implement a simple macro system for syntactic abstraction.

## Phase 4: Tooling and Ecosystem

- [ ] **Language Server Protocol (LSP) Support**: Develop a language server for IDE features (auto-completion, diagnostics).
- [ ] **Code Formatter**: Create an official code formatter (`chtholly fmt`).
- [ ] **Package Manager**: Design a simple package and dependency manager.
- [ ] **Documentation**: Create a website with comprehensive tutorials, language reference, and standard library documentation.
