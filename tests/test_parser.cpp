#include "gtest/gtest.h"
#include "../src/PrattParser.h"
#include "../src/Lexer.h"
#include "../src/AST/ASTPrinter.h"
#include "../src/AST/Stmt.h"

TEST(PrattParserTest, SimpleExpression) {
    std::string source = "1 + 2 * 3;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_FALSE(stmts.empty());
    ASTPrinter printer;
    std::string result = printer.print(stmts[0].get());
    ASSERT_EQ(result, "(+ 1 (* 2 3))");
}

TEST(PrattParserTest, GroupingExpression) {
    std::string source = "(1 + 2) * 3;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_FALSE(stmts.empty());
    ASTPrinter printer;
    std::string result = printer.print(stmts[0].get());
    ASSERT_EQ(result, "(* (group (+ 1 2)) 3)");
}

TEST(PrattParserTest, UnaryExpression) {
    std::string source = "-1;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_FALSE(stmts.empty());
    ASTPrinter printer;
    std::string result = printer.print(stmts[0].get());
    ASSERT_EQ(result, "(- 1)");
}
