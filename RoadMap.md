# Chtholly Development Roadmap

## Phase 1: Project Setup and Core Infrastructure

- [x] Create `RoadMap.md` to track project progress.
- [x] Create `build.py` for automating the CMake build process.
- [x] Set up the initial project structure with `src` and `tests` directories.
- [x] Create the root `CMakeLists.txt` file.
- [x] Configure `.gitignore` to exclude build artifacts.

## Phase 2: Lexer and Tokenization

- [x] Implement the Lexer to tokenize the Chtholly source code.
- [x] Define all the tokens in `Token.h`.
- [x] Write tests for the Lexer to ensure it correctly tokenizes all language constructs.

## Phase 3: Parser and Abstract Syntax Tree (AST)

- [x] Implement the Parser to build an AST from the token stream.
- [x] Define the AST node structures in `AST.h`.
- [x] Write tests for the Parser to ensure it correctly parses variable declarations and simple expressions.
- [ ] Extend the parser to support function declarations, calls, and control flow.

## Phase 4: Semantic Analysis

- [x] Implement the Semantic Analyzer to perform type checking and ownership analysis.
- [x] Implement the Symbol Table to keep track of variables and their types.
- [x] Write tests for the Semantic Analyzer to ensure it correctly identifies semantic errors for variable declarations.
- [ ] Extend the semantic analyzer to support functions and control flow.

## Phase 5: C++ Transpilation

- [x] Implement the C++ transpiler to convert the AST to C++ code.
- [x] Write tests for the transpiler to ensure it correctly generates C++ code for variable declarations and simple expressions.
- [ ] Extend the code generator to support functions and control flow.

## Phase 6: Standard Library

- [ ] Implement the `iostream` module.
- [ ] Implement the `filesystem` module.
- [ ] Implement the `operator` module.
- [ ] Implement the `reflect` module.
- [ ] Implement the `meta` module.

## Phase 7: Self-Hosting

- [ ] Rewrite the Chtholly compiler in Chtholly.
