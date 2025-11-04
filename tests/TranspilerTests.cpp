#include <gtest/gtest.h>
#include "Transpiler.h"
#include "Parser.h"
#include "Lexer.h"

using namespace chtholly;

std::string transpile(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    return transpiler.transpile(stmts);
}

TEST(TranspilerTest, BinaryExpression) {
    std::string source = "1 + 2 * 3;";
    std::string expected = "(1.000000 + (2.000000 * 3.000000));\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, UnaryExpression) {
    std::string source = "-1;";
    std::string expected = "(-1.000000);\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, ParenthesizedExpression) {
    std::string source = "(1 + 2) * 3;";
    std::string expected = "((1.000000 + 2.000000) * 3.000000);\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, BlockStatement) {
    std::string source = "{ 1; 2; }";
    std::string expected = "{\n1.000000;\n2.000000;\n}\n";
    EXPECT_EQ(transpile(source), expected);
}

TEST(TranspilerTest, VariableDeclaration) {
    std::string source = "let a = 1; mut b = 2;";
    std::string expected = "const auto a = 1.000000;\nauto b = 2.000000;\n";
    EXPECT_EQ(transpile(source), expected);
}
