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

## Stage 2: Lexer (In Progress)

- [ ] Implement the lexer to tokenize the Chtholly source code.
- [ ] Define all the token types.
- [ ] Handle comments, keywords, identifiers, literals, and operators.
- [ ] Write unit tests for the lexer.

## Stage 3: Parser

- [ ] Implement the parser to build an Abstract Syntax Tree (AST).
- [ ] Define the AST node structures.
- [ ] Implement parsing for variable declarations, expressions, and statements.
- [ ] Write unit tests for the parser.

## Stage 4: Semantic Analysis

- [ ] Implement the semantic analyzer to perform type checking and other static analysis.
- [ ] Implement symbol table management.
- [ ] Handle variable scoping.
- [ ] Write unit tests for the semantic analyzer.

## Stage 5: Code Generation

- [ ] Implement the code generator to transpile the AST to C++.
- [ ] Generate C++ code for all AST nodes.
- [ ] Write unit tests for the code generator.

## Stage 6: Advanced Features

- [ ] Implement support for functions.
- [ ] Implement support for structs.
- [ ] Implement support for generics.
- [ ] Implement support for traits and constraints.
- [ ] Implement support for modules.
