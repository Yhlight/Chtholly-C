# Chtholly Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Core Compiler Infrastructure

*   [ ] Set up the basic project structure (`src`, `tests`, `build`).
*   [ ] Implement the CMake build system.
*   [ ] Create a Python build script (`build.py`).
*   [ ] Set up the GoogleTest framework for testing.

## Phase 2: Lexer (Tokenization)

*   [ ] Define all token types in `src/Token.h`.
*   [ ] Implement the Lexer in `src/Lexer.h` and `src/Lexer.cpp`.
*   [ ] Write comprehensive tests for the Lexer to cover all tokens and edge cases.

## Phase 3: Parser (AST Generation)

*   [ ] Define the Abstract Syntax Tree (AST) node structures in `src/AST.h`.
*   [ ] Implement the Parser in `src/Parser.h` and `src/Parser.cpp`.
*   [ ] Implement an AST printer for debugging and testing.
*   [ ] Write tests for parsing basic language constructs (variables, literals, operators).

## Phase 4: Semantic Analysis

*   [ ] Implement the Symbol Table in `src/SymbolTable.h` and `src/SymbolTable.cpp`.
*   [ ] Implement the Semantic Analyzer (`Sema`) in `src/Sema.h` and `src/Sema.cpp`.
*   [ ] Add type checking for variable declarations and expressions.
*   [ ] Implement scope resolution.
*   [ ] Write tests for semantic validation (type errors, scope errors, etc.).

## Phase 5: Code Generation

*   [ ] Implement the Code Generator (`CodeGen`) in `src/CodeGen.h` and `src/CodeGen.cpp`.
*   [ ] Translate the AST to C++ code.
*   [ ] Write tests to verify the generated C++ code for basic features.

## Phase 6: Language Features (Iterative)

*   [ ] Functions
*   [ ] Control Flow (if, switch, for, while)
*   [ ] Structs
*   [ ] Ownership and Borrowing
*   [ ] Generics
*   [ ] Traits and Impls
*   [ ] Modules (import)
*   [ ] Standard Library Features (iostream, etc.)
