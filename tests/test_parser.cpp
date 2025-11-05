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

TEST(PrattParserTest, ComparisonExpression) {
    std::string source = "1 < 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_FALSE(stmts.empty());
    ASTPrinter printer;
    std::string result = printer.print(stmts[0].get());
    ASSERT_EQ(result, "(< 1 2)");
}

TEST(PrattParserTest, LogicalExpression) {
    std::string source = "true && false;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_FALSE(stmts.empty());
    ASTPrinter printer;
    std::string result = printer.print(stmts[0].get());
    ASSERT_EQ(result, "(&& true false)");
}

TEST(PrattParserTest, UnaryBangExpression) {
    std::string source = "!true;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_FALSE(stmts.empty());
    ASTPrinter printer;
    std::string result = printer.print(stmts[0].get());
    ASSERT_EQ(result, "(! true)");
}

TEST(PrattParserTest, LambdaExpression) {
    std::string source = "[](a: int, b: int) -> int { return a + b; };";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_FALSE(stmts.empty());
    ASTPrinter printer;
    std::string result = printer.print(stmts[0].get());
    ASSERT_EQ(result, "(lambda)");
}

TEST(PrattParserTest, GenericFunction) {
    std::string source = "func foo<T>() {}";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_FALSE(stmts.empty());
    auto* funcStmt = dynamic_cast<FunctionStmt*>(stmts[0].get());
    ASSERT_NE(funcStmt, nullptr);
    ASSERT_EQ(funcStmt->generics.size(), 1);
    ASSERT_EQ(funcStmt->generics[0].lexeme, "T");
}

TEST(PrattParserTest, GenericStruct) {
    std::string source = "struct foo<T> {}";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_FALSE(stmts.empty());
    auto* structStmt = dynamic_cast<StructStmt*>(stmts[0].get());
    ASSERT_NE(structStmt, nullptr);
    ASSERT_EQ(structStmt->generics.size(), 1);
    ASSERT_EQ(structStmt->generics[0].lexeme, "T");
}
