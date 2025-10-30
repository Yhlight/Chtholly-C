# Chtholly Development Roadmap

## Phase 1: Project Setup and Core Infrastructure

- [x] Create `RoadMap.md` to track project progress.
- [ ] Create `build.py` for automating the CMake build process.
- [ ] Set up the initial project structure with `src` and `tests` directories.
- [ ] Create the root `CMakeLists.txt` file.
- [ ] Configure `.gitignore` to exclude build artifacts.

## Phase 2: Lexer and Tokenization

- [x] Implement the Lexer to tokenize the Chtholly source code.
- [x] Define all the tokens in `Token.h`.
- [x] Write tests for the Lexer to ensure it correctly tokenizes all language constructs.

## Phase 3: Parser and Abstract Syntax Tree (AST)

- [x] Implement the Parser to build an AST from the token stream.
- [x] Define the AST node structures in `AST.h`.
- [x] Write tests for the Parser to ensure it correctly parses all language constructs.

## Phase 4: Semantic Analysis

- [x] Implement the Semantic Analyzer to perform type checking and ownership analysis.
- [x] Implement the Symbol Table to keep track of variables and their types.
- [x] Write tests for the Semantic Analyzer to ensure it correctly identifies semantic errors.

## Phase 5: C++ Transpilation

- [ ] Implement the C++ transpiler to convert the AST to C++ code.
- [ ] Write tests for the transpiler to ensure it correctly generates C++ code.

## Phase 6: Standard Library

- [ ] Implement the `iostream` module.
- [ ] Implement the `filesystem` module.
- [ ] Implement the `operator` module.
- [ ] Implement the `reflect` module.
- [ ] Implement the `meta` module.

## Phase 7: Self-Hosting

- [ ] Rewrite the Chtholly compiler in Chtholly.
