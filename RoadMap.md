# Chtholly-cpp RoadMap

This document outlines the development roadmap for the Chtholly programming language.

## Phase 1: Project Setup

- [x] Configure the development environment
- [x] Initialize the project structure
- [x] Set up the build system (CMake and Python script)
- [x] Create the initial `main.cpp` entry point
- [x] Submit the initial project structure to version control

## Phase 2: Lexer Implementation

- [x] Define tokens in `Token.h`
- [x] Implement the lexer in `Lexer.cpp`
- [x] Add tests for the lexer

## Phase 3: Parser Implementation

- [x] Define the Abstract Syntax Tree (AST) in `AST.h`
- [x] Implement the parser in `Parser.cpp`
- [x] Add tests for the parser

## Phase 4: Code Generation

- [x] Implement the code generator in `CodeGen.cpp`
  - [x] Integer, Double, and String Literals
  - [x] Variable Declarations and Identifiers
  - [x] Binary Expressions
  - [x] Return Statements
  - [x] Functions
  - [x] If/Else Expressions
  - [x] While Loops
- [x] Add tests for the code generator
  - [x] Logical Operators
  - [x] Nested Control Flow
  - [x] Floating-Point Numbers

## Phase 5: CLI

- [ ] Implement a command-line interface to compile Chtholly code

## Phase 6: Self-Hosting

- [ ] Re-implement the Chtholly compiler in Chtholly itself
