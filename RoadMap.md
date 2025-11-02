# Chtholly Development Roadmap

This document outlines the planned development stages for the Chtholly programming language. The development will be broken down into phases, starting with foundational components and progressively adding more complex features.

## Phase 1: Project Foundation and Core Compiler

- [x] **Project Scaffolding**
  - [x] Create directory structure (`src`, `tests`, `scripts`).
  - [x] Initialize `CMakeLists.txt` for the main project and tests.
  - [x] Integrate Google Test for unit testing.
  - [x] Create a Python build script (`scripts/build.py`) to automate the build process.
  - [x] Set up `.gitignore` to exclude build artifacts.

- [x] **Core Compiler Components**
  - [x] Implement a centralized `ErrorReporter`.
  - [x] Define `Token` and `TokenType` for lexical analysis.
  - [x] Implement the `Lexer` to scan source code and produce tokens.
  - [x] Design the base AST nodes and Visitor pattern (`Expr.h`, `Stmt.h`).
  - [x] Implement an `ASTPrinter` for debugging and testing.

## Phase 2: Basic Language Features

- [x] **Expressions**
  - [x] Parse and transpile literal expressions (integers, doubles, strings, chars, booleans).
  - [x] Parse and transpile unary and binary expressions, respecting C++ precedence.
  - [x] Parse and transpile grouping parentheses.

- [x] **Statements**
  - [x] Implement `let` (immutable) and `mut` (mutable) variable declarations with type inference.
  - [x] Implement expression statements (e.g., `a = 10;`).
  - [x] Implement a `print` statement (as a language construct for now).
  - [x] Implement the `Transpiler` to wrap top-level statements in a C++ `main` function.
  - [x] Implement the `Resolver` to handle variable declarations and scope.

## Phase 3: Control Flow

- [x] Implement `if`/`else` statements.
- [x] Implement `Block` statements (`{ ... }`) and corresponding scope handling in the `Resolver`.
- [x] Implement `while` loops.
- [x] Implement `switch` statements with `case` and `fallthrough`.

## Phase 4: Functions

- [x] Implement `func` statement parsing for function declarations.
- [x] Implement function call expressions.
- [x] Implement `return` statements.
- [x] Update `Resolver` to handle function scopes and parameters.
- [ ] (Stretch Goal) Implement Lambda function expressions.

## Phase 5: Data Structures

- [x] Implement `struct` declarations, including member variables and methods.
- [x] Implement struct instantiation and field access (`.`).
- [x] Implement `self` keyword (`self`, `&self`, `&mut self`).
- [ ] Handle static-like methods (no `self` parameter).
- [ ] Implement `enum` declarations.
- [ ] Implement basic array support (declaration and initialization).

## Phase 6: Ownership and References

- [ ] Implement syntax for immutable (`&`) and mutable (`&mut`) references.
- [ ] Begin semantic analysis in the `Resolver` for basic ownership rules (e.g., preventing assignment to `let` variables and immutable references).
- [ ] Differentiate between `move` and `copy` semantics for different types during transpilation.

## Phase 7: Advanced Types and Generics

- [ ] Implement `option<T>` and `result<T, K>` types.
- [ ] Implement generic functions (`func <name><T>...`).
- [ ] Implement generic structs (`struct <name><T>...`).
- [ ] Implement traits and generic constraints (`?`).

## Phase 8: Modularity and Standard Library

- [ ] Implement `import` statements for file-based modules.
- [ ] Create a basic `iostream` module with `print` and `input`.
- [ ] Implement operator overloading via the `operator` module.
- [ ] Implement built-in `type_cast<T>()` function.

## Phase 9: Metaprogramming

- [ ] Implement the `reflect` module for static reflection.
- [ ] Implement the `meta` module for type diagnostics.
- [ ] Implement the `util` module for utility functions.

## Phase 10: Self-Hosting

- [ ] Rewrite the Chtholly compiler in the Chtholly language itself.
- [ ] Expand language features and standard library.
