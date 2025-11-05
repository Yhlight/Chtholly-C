# Chtholly Project Roadmap

This document outlines the development progress and future goals for the Chtholly programming language.

## Phase 1: Project Setup and Foundation

- [x] Create project directory structure (`src`, `tests`, `scripts`)
- [x] Establish build system with CMake and Python build script
- [x] Implement core data structures (Token, TypeInfo)

## Phase 2: Lexer

- [x] Implement the Lexer to tokenize the Chtholly source code.
- [x] Add support for all keywords, operators, and literals defined in `Chtholly.md`.
- [x] Write comprehensive tests for the Lexer.

## Phase 3: Parser

- [x] Implement the Parser to build an Abstract Syntax Tree (AST).
- [x] Define the AST node hierarchy in `src/AST.h`.
- [x] Implement parsing for all language constructs (variables, functions, control flow, etc.).
- [x] Write comprehensive tests for the Parser.

## Phase 4: Transpiler

- [ ] Implement the Transpiler to convert the AST to C++ code.
  - [x] Expression and basic statement transpilation
- [x] Implement visitors for all AST node types.
- [ ] Add support for all language features, including generics, traits, and modules.
- [x] Write comprehensive tests for the Transpiler.

## Phase 5: Advanced Features

- [ ] Implement the standard library (`iostream`, `filesystem`, etc.).
  - [x] Basic module import system
  - [x] `iostream` module with `print` function
  - [x] `filesystem` module with `fs_read` and `fs_write`
- [ ] Implement static reflection and metaprogramming features.
- [ ] Implement operator overloading.
