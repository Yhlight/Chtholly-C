#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AstPrinter.h"

using namespace chtholly;

std::string parseAndPrint(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    AstPrinter printer;
    return printer.print(stmts);
}

TEST(ParserTest, BinaryExpression) {
    std::string source = "1 + 2 * 3;";
    std::string expected = "(+ 1.000000 (* 2.000000 3.000000))\n";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, UnaryExpression) {
    std::string source = "-1;";
    std::string expected = "(- 1.000000)\n";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, ParenthesizedExpression) {
    std::string source = "(1 + 2) * 3;";
    std::string expected = "(* (+ 1.000000 2.000000) 3.000000)\n";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, BlockStatement) {
    std::string source = "{ 1; 2; }";
    std::string expected = "(block 1.000000 2.000000)\n";
    EXPECT_EQ(parseAndPrint(source), expected);
}
