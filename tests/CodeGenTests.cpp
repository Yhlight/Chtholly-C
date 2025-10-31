#include <gtest/gtest.h>
#include "CodeGen.h"
#include "Parser.h"
#include "Lexer.h"

using namespace chtholly;

TEST(CodeGenTest, SimpleVariableDeclaration) {
    std::string source = "let a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    CodeGen codeGen;
    std::string expected = "const auto a = 10;\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, MutableVariableDeclaration) {
    std::string source = "mut b = 20;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    CodeGen codeGen;
    std::string expected = "auto b = 20;\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, SimpleBinaryExpression) {
    std::string source = "let c = 10 + 20;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    CodeGen codeGen;
    std::string expected = "const auto c = (10 + 20);\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, FunctionDeclaration) {
    std::string source = "func add(x: int, y: int) -> int { let z = x + y; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    CodeGen codeGen;
    std::string expected = "int add(int x, int y) {\nconst auto z = (x + y);\n}\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, FunctionCall) {
    std::string source = "func add(x: int, y: int) -> int { let z = x + y; }\nlet a = add(1, 2);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    CodeGen codeGen;
    std::string expected = "int add(int x, int y) {\nconst auto z = (x + y);\n}\nconst auto a = add(1, 2);\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, IfStatement) {
    std::string source = "if (true) { let a = 1; } else { let b = 2; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    CodeGen codeGen;
    std::string expected = "if (true) {\nconst auto a = 1;\n}\nelse {\nconst auto b = 2;\n}\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, SwitchStatement) {
    std::string source = "switch (x) { case 1: { let a = 1; } case 2: { let b = 2; } }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    CodeGen codeGen;
    std::string expected = "switch (x) {\ncase 1: {\nconst auto a = 1;\n}\ncase 2: {\nconst auto b = 2;\n}\n}\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, SwitchStatementWithDefault) {
    std::string source = "switch (x) { case 1: { let a = 1; } default: { let b = 2; } }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    CodeGen codeGen;
    std::string expected = "switch (x) {\ncase 1: {\nconst auto a = 1;\n}\ndefault: {\nconst auto b = 2;\n}\n}\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}

TEST(CodeGenTest, EnumDeclaration) {
    std::string source = "enum Color { Red, Green, Blue };";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    CodeGen codeGen;
    std::string expected = "enum class Color {\n    Red,\n    Green,\n    Blue\n};\n";
    EXPECT_EQ(codeGen.generate(*ast), expected);
}
