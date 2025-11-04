# Chtholly Project Roadmap

This document outlines the development roadmap for the Chtholly programming language.

## Phase 1: Project Setup (In Progress)

- [ ] Set up the development environment.
- [ ] Create the initial project structure (`src`, `tests`, `scripts`).
- [ ] Initialize the CMake build system.
- [ ] Set up GoogleTest for unit testing.
- [ ] Create a Python build script.
- [ ] Add initial placeholder source files.
- [ ] Configure `.gitignore`.

## Phase 2: Lexer Implementation

- [ ] Define the token types in `src/Token.h`.
- [ ] Implement the Lexer in `src/Lexer.h` and `src/Lexer.cpp`.
- [ ] Add tests for the Lexer to verify that it correctly tokenizes the source code.

## Phase 3: Parser Implementation

- [ ] Define the AST nodes in `src/AST.h`.
- [ ] Implement the Parser in `src/Parser.h` and `src/Parser.cpp`.
- [ ] Add tests for the Parser to verify that it correctly constructs the AST.

## Phase 4: Transpiler Implementation

- [ ] Implement the Transpiler in `src/Transpiler.h` and `src/Transpiler.cpp`.
- [ ] Add tests for the Transpiler to verify that it correctly generates C++ code.
