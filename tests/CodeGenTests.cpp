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
