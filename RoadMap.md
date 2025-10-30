# Chtholly Development Roadmap

## Phase 1: Project Setup and Core Infrastructure

- [x] Set up the basic project structure (`src`, `tests`, `build.py`, `CMakeLists.txt`).
- [ ] Implement the command-line interface for the compiler.
- [ ] Set up the testing framework.

## Phase 2: Lexer and Parser

- [ ] Implement the lexer to tokenize the Chtholly source code.
- [ ] Implement the parser to build an Abstract Syntax Tree (AST) from the tokens.
- [ ] Add basic error handling for syntax errors.

## Phase 3: C++ Transpiler

- [ ] Implement the C++ code generator (transpiler) that traverses the AST.
- [ ] Translate basic Chtholly syntax (variables, functions, control flow) to C++.
- [ ] Handle Chtholly's ownership and borrowing rules in the transpiled code.

## Phase 4: Feature Implementation

- [ ] Implement support for structs and methods.
- [ ] Implement generics.
- [ ] Implement traits and constraints.
- [ ] Implement the standard library modules (`iostream`, `filesystem`, etc.).

## Phase 5: Self-Hosting

- [ ] Rewrite the Chtholly compiler in Chtholly.
- [ ] Bootstrap the new compiler using the C++-based compiler.
