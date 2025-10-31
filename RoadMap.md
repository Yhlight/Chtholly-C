# Chtholly Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Project Setup and Core Infrastructure
- [x] Set up the basic project structure (`src`, `tests`, `scripts`).
- [x] Implement the CMake build system.
- [x] Create a Python build script (`scripts/build.py`).
- [x] Set up Google Test for unit testing.
- [x] Create a basic "Hello, World!" entry point to verify the build.

## Phase 2: Lexical Analysis (Lexer)
- [x] Implement the Lexer to tokenize Chtholly source code.
- [x] Define all necessary tokens (keywords, operators, literals, etc.).
- [x] Write unit tests for the Lexer.

## Phase 3: Syntactic Analysis (Parser)
- [x] Design and implement the Abstract Syntax Tree (AST) nodes.
- [x] Implement a recursive descent parser.
- [x] Handle basic declarations (variables, functions).
- [x] Implement expression parsing (operators, precedence).
- [x] Write unit tests for the Parser.

## Phase 4: C++ Transpiler
- [x] Create a C++ code generator that traverses the AST.
- [x] Translate Chtholly AST nodes into equivalent C++ code.
- [x] Handle basic code generation for variables, functions, and expressions.
- [x] Write unit tests for the transpiler.

## Phase 5: Semantic Analysis
- [x] Implement initial Type System for Semantic Analysis
- [x] Implement a symbol table.
- [x] Perform type checking.
- [x] Enforce ownership and borrowing rules.
- [x] Write unit tests for the semantic analyzer.

## Phase 6: Advanced Features
- [ ] Implement structs and methods.
- [ ] Implement generics.
- [ ] Implement traits and constraints.
- [ ] Implement modules and the import system.

## Phase 7: Standard Library
- [ ] Implement the `iostream` module.
- [ ] Implement the `filesystem` module.
- [ ] Implement the `operator` module for operator overloading.
- [ ] Implement the `reflect` module for static reflection.
- [ ] Implement the `meta` module for metaprogramming.

## Phase 8: Self-Hosting
- [ ] Write a Chtholly compiler in Chtholly itself.
