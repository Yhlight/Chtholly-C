#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AST/ASTPrinter.h"
#include <vector>
#include <memory>

static std::string parseAndPrint(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    if (stmts.empty()) return "";
    ASTPrinter printer;
    // For simplicity in these tests, we assume one statement per test case
    if(stmts[0] == nullptr) return "ParseError";
    return printer.print(*stmts[0]);
}

TEST(ParserTest, ParsesLiteral) {
    std::string result = parseAndPrint("123;");
    EXPECT_EQ(result, "(; 123.000000)");
}

TEST(ParserTest, ParsesUnary) {
    std::string result = parseAndPrint("-123;");
    EXPECT_EQ(result, "(; (- 123.000000))");
}

TEST(ParserTest, ParsesBinaryWithCorrectPrecedence) {
    std::string result = parseAndPrint("1 + 2 * 3;");
    EXPECT_EQ(result, "(; (+ 1.000000 (* 2.000000 3.000000)))");
}

TEST(ParserTest, ParsesGrouping) {
    std::string result = parseAndPrint("(1 + 2) * 3;");
    EXPECT_EQ(result, "(; (* (group (+ 1.000000 2.000000)) 3.000000))");
}

TEST(ParserTest, ParsesVariable) {
    std::string result = parseAndPrint("my_var;");
    EXPECT_EQ(result, "(; my_var)");
}

TEST(ParserTest, ParsesAssignment) {
    std::string result = parseAndPrint("my_var = 10;");
    EXPECT_EQ(result, "(; (= my_var 10.000000))");
}

TEST(ParserTest, ParsesLetDeclaration) {
    std::string result = parseAndPrint("let a = 10;");
    EXPECT_EQ(result, "(let a = 10.000000)");
}

TEST(ParserTest, ParsesPrintStatement) {
    std::string result = parseAndPrint("print 10;");
    EXPECT_EQ(result, "(print 10.000000)");
}

TEST(ParserTest, ParsesIfStatement) {
    std::string result = parseAndPrint("if (true) print 1;");
    EXPECT_EQ(result, "(if true (print 1.000000))");
}

TEST(ParserTest, ParsesIfElseStatement) {
    std::string result = parseAndPrint("if (true) print 1; else print 2;");
    EXPECT_EQ(result, "(if-else true (print 1.000000) (print 2.000000))");
}

TEST(ParserTest, ParsesWhileStatement) {
    std::string result = parseAndPrint("while (true) print 1;");
    EXPECT_EQ(result, "(while true (print 1.000000))");
}
