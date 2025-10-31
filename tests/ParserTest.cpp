#include <gtest/gtest.h>
#include "Parser.h"
#include "AST.h"

TEST(ParserTest, ParsesExpressionStatement) {
    std::vector<chtholly::Token> tokens = {
        chtholly::Token(chtholly::TokenType::NUMBER, "123", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::END_OF_FILE, "", 1)
    };
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    // Check if the parsed statement is an ExprStmt
    auto* exprStmt = dynamic_cast<chtholly::ExprStmt*>(statements[0].get());
    ASSERT_NE(exprStmt, nullptr);

    // Check if the expression within the statement is a LiteralExpr
    auto* literalExpr = dynamic_cast<chtholly::LiteralExpr*>(exprStmt->expression.get());
    ASSERT_NE(literalExpr, nullptr);
    EXPECT_EQ(literalExpr->token.lexeme, "123");
}

TEST(ParserTest, ParsesPrecedence) {
    // Expression: -1 + 2 * 3;
    // Expected AST: (+ (- 1) (* 2 3))
    std::vector<chtholly::Token> tokens = {
        chtholly::Token(chtholly::TokenType::MINUS, "-", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "1", 1),
        chtholly::Token(chtholly::TokenType::PLUS, "+", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "2", 1),
        chtholly::Token(chtholly::TokenType::STAR, "*", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "3", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::END_OF_FILE, "", 1)
    };
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    chtholly::AstPrinter printer;
// For now, we assume the statement is an expression statement.
auto* exprStmt = dynamic_cast<chtholly::ExprStmt*>(statements[0].get());
ASSERT_NE(exprStmt, nullptr);
std::string result = printer.print(*exprStmt->expression);
    EXPECT_EQ(result, "(+ (- 1) (* 2 3))");
}

TEST(ParserTest, ParsesVarDeclaration) {
    // let a = 10;
    std::vector<chtholly::Token> tokens = {
        chtholly::Token(chtholly::TokenType::LET, "let", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "a", 1),
        chtholly::Token(chtholly::TokenType::EQUAL, "=", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "10", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::END_OF_FILE, "", 1)
    };
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    auto* varDeclStmt = dynamic_cast<chtholly::VarDeclStmt*>(statements[0].get());
    ASSERT_NE(varDeclStmt, nullptr);
    EXPECT_EQ(varDeclStmt->name.lexeme, "a");
}
