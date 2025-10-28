# Chtholly Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Project Setup and Core Infrastructure (In Progress)

- [x] Set up the development environment.
- [x] Create the `.gitignore` file.
- [x] Create the `RoadMap.md` file.
- [x] Create the Python build script.
- [x] Create the initial C++ project structure.
- [x] Create the first test case.

## Phase 2: Lexer and Parser

- [ ] Implement the lexer to tokenize Chtholly source code.
- [ ] Implement the parser to build an Abstract Syntax Tree (AST).
- [ ] Define the AST nodes.
- [ ] Implement basic error handling and reporting.

## Phase 3: Semantic Analysis and Type System

- [ ] Implement a symbol table to track variables, functions, and types.
- [ ] Implement a type checker to enforce Chtholly's type rules.
- [ ] Implement the ownership, borrowing, and lifetime rules.

## Phase 4: Code Generation with LLVM

- [ ] Set up the LLVM backend.
- [ ] Generate LLVM Intermediate Representation (IR) for basic expressions.
- [ ] Generate LLVM IR for control flow (if, for, while, switch).
- [ ] Generate LLVM IR for functions and structs.

## Phase 5: Standard Library Implementation

- [ ] Implement the `iostream` module for basic input/output.
- [ ] Implement the `filesystem` module for file operations.
- [ ] Implement the `operator` module for operator overloading.
- [ ] Implement the `reflect` module for static reflection.
- [ ] Implement the `meta` module for metaprogramming.

## Phase 6: Self-Hosting and Advanced Features

- [ ] Begin writing a new Chtholly compiler in the Chtholly language itself.
- [ ] Expand the language with more advanced features as needed.
