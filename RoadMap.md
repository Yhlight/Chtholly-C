# Chtholly Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Core Compiler Infrastructure

*   [x] Set up the basic project structure (`src`, `tests`, `build`).
*   [x] Implement the CMake build system.
*   [x] Create a Python build script (`build.py`).
*   [x] Set up the GoogleTest framework for testing.

## Phase 2: Lexer (Tokenization)

*   [x] Define all token types in `src_Token.h`.
*   [x] Implement the Lexer in `src_Lexer.h` and `src_Lexer.cpp`.
*   [x] Write comprehensive tests for the Lexer to cover all tokens and edge cases.

## Phase 3: Parser (AST Generation)

*   [x] Define the Abstract Syntax Tree (AST) node structures in `src_AST.h`.
*   [x] Implement the Parser in `src_Parser.h` and `src_Parser.cpp`.
*   [x] Implement an AST printer for debugging and testing.
*   [x] Write tests for parsing basic language constructs (variables, literals, operators).

## Phase 4: Semantic Analysis

*   [x] Implement the Symbol Table in `src_SymbolTable.h` and `src_SymbolTable.cpp`.
*   [x] Implement the Semantic Analyzer (`Sema`) in `src_Sema.h` and `src_Sema.cpp`.
*   [x] Add type checking for variable declarations and expressions.
*   [x] Implement scope resolution.
*   [x] Write tests for semantic validation (type errors, scope errors, etc.).

## Phase 5: Code Generation

*   [ ] Implement the Code Generator (`CodeGen`) in `src_CodeGen.h` and `src_CodeGen.cpp`.
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
