# Chtholly Project Roadmap

This document outlines the development progress and future goals for the Chtholly programming language.

## Phase 1: Project Setup and Foundation

- [x] Create project directory structure (`src`, `tests`, `scripts`)
- [x] Establish build system with CMake and Python build script
- [x] Implement core data structures (Token, TypeInfo)

## Phase 2: Lexer

- [ ] Implement the Lexer to tokenize the Chtholly source code.
- [ ] Add support for all keywords, operators, and literals defined in `Chtholly.md`.
- [ ] Write comprehensive tests for the Lexer.

## Phase 3: Parser

- [ ] Implement the Parser to build an Abstract Syntax Tree (AST).
- [ ] Define the AST node hierarchy in `src/AST.h`.
- [ ] Implement parsing for all language constructs (variables, functions, control flow, etc.).
- [ ] Write comprehensive tests for the Parser.

## Phase 4: Transpiler

- [ ] Implement the Transpiler to convert the AST to C++ code.
- [ ] Implement visitors for all AST node types.
- [ ] Add support for all language features, including generics, traits, and modules.
- [ ] Write comprehensive tests for the Transpiler.

## Phase 5: Advanced Features

- [ ] Implement the standard library (`iostream`, `filesystem`, etc.).
- [ ] Implement static reflection and metaprogramming features.
- [ ] Implement operator overloading.
