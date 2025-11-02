#include "gtest/gtest.h"
#include "../src/Parser.h"
#include "../src/Lexer.h"
#include "../src/AST/ASTPrinter.h"
#include "../src/AST/Stmt.h"

TEST(ParserTest, SimpleExpression) {
    std::string source = "1 + 2 * 3;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
    ASSERT_NE(exprStmt, nullptr);
    ASTPrinter printer;
    std::string result = printer.print(*exprStmt->expression);
    ASSERT_EQ(result, "(+ 1 (* 2 3))");
}

TEST(ParserTest, GroupingExpression) {
    std::string source = "(1 + 2) * 3;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
    ASSERT_NE(exprStmt, nullptr);
    ASTPrinter printer;
    std::string result = printer.print(*exprStmt->expression);
    ASSERT_EQ(result, "(* (group (+ 1 2)) 3)");
}

TEST(ParserTest, LetStatement) {
    std::string source = "let x = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    LetStmt* letStmt = dynamic_cast<LetStmt*>(statements[0].get());
    ASSERT_NE(letStmt, nullptr);
    EXPECT_EQ(letStmt->name.lexeme, "x");
}

TEST(ParserTest, PrintStatement) {
    std::string source = "print 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    PrintStmt* printStmt = dynamic_cast<PrintStmt*>(statements[0].get());
    ASSERT_NE(printStmt, nullptr);
}

TEST(ParserTest, MutLetStatement) {
    std::string source = "mut x = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    LetStmt* letStmt = dynamic_cast<LetStmt*>(statements[0].get());
    ASSERT_NE(letStmt, nullptr);
    EXPECT_EQ(letStmt->name.lexeme, "x");
    EXPECT_TRUE(letStmt->isMutable);
}

TEST(ParserTest, AssignmentExpression) {
    std::string source = "x = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    ExpressionStmt* stmt = dynamic_cast<ExpressionStmt*>(statements[0].get());
    ASSERT_NE(stmt, nullptr);
    AssignExpr* expr = dynamic_cast<AssignExpr*>(stmt->expression.get());
    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->name.lexeme, "x");
}
