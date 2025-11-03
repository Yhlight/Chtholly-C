# Chtholly Development Roadmap

This document outlines the planned development phases for the Chtholly programming language.

## Phase 1: Core Compiler Infrastructure
- [ ] **Project Setup**: Establish the basic directory structure, build system (CMake), and helper scripts.
- [ ] **Lexer (Tokenizer)**: Implement the lexical analyzer to convert source code into a stream of tokens.
- [ ] **Parser & AST**: Implement a parser to consume tokens and produce an Abstract Syntax Tree (AST). Define the initial set of AST nodes for expressions and statements.
- [ ] **Error Reporting**: Implement a basic error reporting mechanism to handle syntax errors.

## Phase 2: Semantic Analysis & Type System
- [ ] **Resolver (Semantic Analyzer)**: Implement a resolver to perform semantic checks on the AST.
- [ ] **Symbol Tables & Scoping**: Implement symbol tables to manage variable and function scopes.
- [ ] **Type Checking & Inference**: Implement type checking for expressions and statements, along with basic type inference.
- [ ] **Ownership & Borrow Checker**: Design and implement the core ownership and borrowing rules.

## Phase 3: C++ Transpiler & Execution
- [ ] **Transpiler**: Implement the C++ code generator that translates the AST into valid C++ code.
- [ ] **Execution Engine**: Integrate the transpiler with a C++ compiler (like g++) to compile and run the generated code.
- [ ] **Basic Language Features**: Transpile and execute simple expressions, variable declarations, and function calls.

## Phase 4: Expanding Language Features
- [ ] **Control Flow**: Implement `if/else`, `while`, and `switch` statements.
- [ ] **Structs**: Implement struct definitions and instantiations.
- [ ] **Functions**: Full support for function definitions, including parameters and return types.
- [ ] **Modules**: Implement the `import` statement for file-based modules.

## Phase 5: Advanced Features & Standard Library
- [ ] **Traits & Impls**: Implement traits for defining shared behavior and `impl` blocks for implementing them.
- [ ] **Generics**: Implement generic functions and structs.
- [ ] **Operator Overloading**: Implement support for operator overloading through traits.
- [ ] **Standard Library**: Begin implementation of the `iostream`, `filesystem`, and other core library modules.

## Phase 6: Self-Hosting & Beyond
- [ ] **Self-Hosting Compiler**: Begin the process of rewriting the Chtholly compiler in Chtholly itself.
- [ ] **Metaprogramming & Reflection**: Implement the `meta` and `reflect` modules.
