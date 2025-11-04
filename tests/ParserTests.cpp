#include <gtest/gtest.h>
#include "Parser.h"
#include "AstPrinter.h"
#include "Lexer.h"
#include <vector>
#include <string>

using namespace chtholly;

std::string parseAndPrint(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    AstPrinter printer;
    return printer.print(statements);
}

TEST(ParserTest, BasicExpression) {
    std::string source = "1 + 2 * 3;";
    std::string expected = "(; (+ 1 (* 2 3)))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, ParenthesizedExpression) {
    std::string source = "(1 + 2) * 3;";
    std::string expected = "(; (* (group (+ 1 2)) 3))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, UnaryExpression) {
    std::string source = "-1 * -2;";
    std::string expected = "(; (* (- 1) (- 2)))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, VariableDeclaration) {
    std::string source = "let x = 10;";
    std::string expected = "(let x 10)";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, MutableVariableDeclaration) {
    std::string source = "mut y = \"hello\";";
    std::string expected = "(mut y hello)";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, MultipleStatements) {
    std::string source = "let x = 1; let y = 2;";
    std::string expected = "(let x 1)(let y 2)";
    EXPECT_EQ(parseAndPrint(source), expected);
}
