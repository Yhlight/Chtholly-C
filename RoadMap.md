# Chtholly Compiler Development Roadmap

This document outlines the development plan for the Chtholly compiler.

## Stage 1: Project Setup

- [x] Set up the development environment.
- [x] Create the basic directory structure (`src/`, `include/`).
- [x] Create the `RoadMap.md` file.
- [x] Create the `CMakeLists.txt` file.
- [x] Create the `build.py` script.
- [x] Create a placeholder `main.cpp` file.
- [x] Update the `.gitignore` file.

## Stage 2: Lexer

- [x] Implement the lexer to tokenize the Chtholly source code.
- [x] Define all the token types.
- [x] Handle comments, keywords, identifiers, literals, and operators.
- [x] Write unit tests for the lexer.

## Stage 3: Parser

- [x] Implement the parser to build an Abstract Syntax Tree (AST).
- [x] Define the AST node structures.
- [x] Implement parsing for variable declarations, expressions, and statements.
- [x] Write unit tests for the parser.

## Stage 4: Semantic Analysis

- [x] Implement symbol table management.
- [x] Handle variable scoping.
- [x] Write unit tests for the semantic analyzer.
- [ ] Implement type checking.

## Stage 5: Code Generation

- [x] Implement the code generator to transpile the AST to C++.
- [x] Generate C++ code for all AST nodes.
- [x] Write unit tests for the code generator.

## Stage 6: Advanced Features (In Progress)

- [ ] Implement support for `if-else` control flow.
- [ ] Implement support for functions.
- [ ] Implement support for structs.
- [ ] Implement support for generics.
- [ ] Implement support for traits and constraints.
- [ ] Implement support for modules.
