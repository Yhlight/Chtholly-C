# Chtholly Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Core Compiler Infrastructure

- [x] **Lexer:** Implement a lexical analyzer to tokenize the Chtholly source code.
- [ ] **Parser:** Implement a parser to build an Abstract Syntax Tree (AST) from the token stream.
  - [x] Variable Declarations
  - [x] Expressions
  - [x] Control Flow (if/else)
- [ ] **Semantic Analysis:** Perform semantic checks on the AST to ensure correctness.
- [ ] **Code Generation:** Generate LLVM Intermediate Representation (IR) from the AST.
- [ ] **Compiler Driver:** Create a command-line interface to drive the compilation process.

## Phase 2: Language Features

- [ ] Variables and Data Types
- [ ] Operators
- [ ] Control Flow (if, for, while, switch)
- [ ] Functions
- [ ] Structs
- [ ] Ownership and Borrowing
- [ ] Generics
- [ ] Modules

## Phase 3: Standard Library

- [ ] iostream
- [ ] filesystem
- [ ] operator
- [ ] reflect
- [ ] meta

## Phase 4: Self-Hosting

- [ ] Rewrite the Chtholly compiler in Chtholly.
