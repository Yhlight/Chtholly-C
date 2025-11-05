#include <gtest/gtest.h>
#include "Transpiler.h"
#include "Parser.h"
#include "Lexer.h"
#include <vector>
#include <string>

using namespace chtholly;

std::string transpile(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    Transpiler transpiler;
    return transpiler.transpile(statements);
}

TEST(TranspilerTest, VariableDeclaration) {
    std::string source = "let x = 10;";
    std::string expected = "const auto x = 10;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, MutableVariableDeclaration) {
    std::string source = "mut y = \"hello\";";
    std::string expected = "auto y = \"hello\";\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, ExpressionStatement) {
    std::string source = "1 + 2 * 3;";
    std::string expected = "1 + 2 * 3;\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, BlockStatement) {
    std::string source = "{ let a = 1; mut b = 2; }";
    std::string expected = "{\nconst auto a = 1;\nauto b = 2;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, IfStatement) {
    std::string source = "if (x > 0) { x = x - 1; }";
    std::string expected = "if (x > 0) {\nx = x - 1;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, IfElseStatement) {
    std::string source = "if (x > 0) { x = 1; } else { x = 2; }";
    std::string expected = "if (x > 0) {\nx = 1;\n}\nelse {\nx = 2;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, WhileStatement) {
    std::string source = "while (x > 0) { x = x - 1; }";
    std::string expected = "while (x > 0) {\nx = x - 1;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, SwitchStatement) {
    std::string source = "switch (x) { case 1: { y = 1; break; } default: { y = 2; } }";
    std::string expected = "{\nauto&& __switch_val = x;\nif (__switch_val == 1) {\ny = 1;\nbreak;\n}\nelse {\ny = 2;\n}\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, SwitchFallthroughStatement) {
    std::string source = "switch (x) { case 1: { y = 1; fallthrough; } case 2: { y = 2; } }";
    std::string expected = "{\nauto&& __switch_val = x;\nif (__switch_val == 1 || __switch_val == 2) {\ny = 1;\ny = 2;\n}\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, SwitchMultipleFallthroughStatement) {
    std::string source = "switch (x) { case 1: { y = 1; fallthrough; } case 2: { y = 2; fallthrough; } case 3: { y = 3; } }";
    std::string expected = "{\nauto&& __switch_val = x;\nif (__switch_val == 1 || __switch_val == 2 || __switch_val == 3) {\ny = 1;\ny = 2;\ny = 3;\n}\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, FunctionDeclaration) {
    std::string source = "func add(a: int, b: int) -> int { return a + b; }";
    std::string expected = "int add(int a, int b) {\nreturn a + b;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, StructDeclaration) {
    std::string source = "struct Point { let x: int; let y: int; }";
    std::string expected = "struct Point {\nint x;\nint y;\n};\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, StructWithMethod) {
    std::string source = "struct Point { let x: int; move(dx: int) -> void { x = x + dx; } }";
    std::string expected = "struct Point {\nint x;\nvoid move(int dx) {\nx = x + dx;\n}\n};\n";
    EXPECT_EQ(transpile(source), expected);
}
