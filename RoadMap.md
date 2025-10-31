# Chtholly Development Roadmap

## Phase 1: Project Setup and Core Infrastructure

- [x] Set up the basic project structure.
- [x] Implement the command-line interface for the compiler.
- [x] Set up the testing framework.

## Phase 2: Lexer and Parser

- [x] Implement the lexer to tokenize the Chtholly source code.
- [x] Implement the LL(k) parser to build an Abstract Syntax Tree (AST) from the tokens.
- [x] Add basic error handling for syntax errors.

## Phase 3: C++ Transpiler

- [x] Implement the C++ code generator (transpiler) that traverses the AST.
- [x] Translate basic Chtholly syntax (variables, functions, control flow) to C++.
- [x] Handle Chtholly's ownership and borrowing rules in the transpiled code (Mutable Variables (`mut`)).

## Phase 4: Feature Implementation

- [x] Implement support for structs and methods.
- [x] Implement generics (Generic Structs).
- [x] Implement enums.
- [x] Implement traits and constraints.
- [x] Implement the standard library modules (`iostream`, `filesystem`, etc.).
  - [x] `iostream`
  - [x] `filesystem`

## Phase 5: Self-Hosting

- [ ] Rewrite the Chtholly compiler in Chtholly.
- [ ] Bootstrap the new compiler using the C++-based compiler.
