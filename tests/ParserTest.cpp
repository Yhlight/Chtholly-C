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

TEST(ParserTest, ParsesGrouping) {
    // (1 + 2) * 3;
    std::vector<chtholly::Token> tokens = {
        chtholly::Token(chtholly::TokenType::LEFT_PAREN, "(", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "1", 1),
        chtholly::Token(chtholly::TokenType::PLUS, "+", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "2", 1),
        chtholly::Token(chtholly::TokenType::RIGHT_PAREN, ")", 1),
        chtholly::Token(chtholly::TokenType::STAR, "*", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "3", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::END_OF_FILE, "", 1)
    };
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    chtholly::AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(; (* (group (+ 1 2)) 3))");
}

TEST(ParserTest, ParsesIfStatement) {
    // if (true) { 1; } else { 2; }
    std::vector<chtholly::Token> tokens = {
        chtholly::Token(chtholly::TokenType::IF, "if", 1),
        chtholly::Token(chtholly::TokenType::LEFT_PAREN, "(", 1),
        chtholly::Token(chtholly::TokenType::TRUE, "true", 1),
        chtholly::Token(chtholly::TokenType::RIGHT_PAREN, ")", 1),
        chtholly::Token(chtholly::TokenType::LEFT_BRACE, "{", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "1", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::RIGHT_BRACE, "}", 1),
        chtholly::Token(chtholly::TokenType::ELSE, "else", 1),
        chtholly::Token(chtholly::TokenType::LEFT_BRACE, "{", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "2", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::RIGHT_BRACE, "}", 1),
        chtholly::Token(chtholly::TokenType::END_OF_FILE, "", 1)
    };
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    chtholly::AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(if true (block (; 1)) (block (; 2)))");
}

TEST(ParserTest, ParsesForStatement) {
    // for (let i = 0; i < 10; i = i + 1) { 1; }
    std::vector<chtholly::Token> tokens = {
        chtholly::Token(chtholly::TokenType::FOR, "for", 1),
        chtholly::Token(chtholly::TokenType::LEFT_PAREN, "(", 1),
        chtholly::Token(chtholly::TokenType::LET, "let", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "i", 1),
        chtholly::Token(chtholly::TokenType::EQUAL, "=", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "0", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "i", 1),
        chtholly::Token(chtholly::TokenType::LESS, "<", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "10", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "i", 1),
        chtholly::Token(chtholly::TokenType::EQUAL, "=", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "i", 1),
        chtholly::Token(chtholly::TokenType::PLUS, "+", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "1", 1),
        chtholly::Token(chtholly::TokenType::RIGHT_PAREN, ")", 1),
        chtholly::Token(chtholly::TokenType::LEFT_BRACE, "{", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "1", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::RIGHT_BRACE, "}", 1),
        chtholly::Token(chtholly::TokenType::END_OF_FILE, "", 1)
    };
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    chtholly::AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(for (var i = 0) (< i 10) (= i (+ i 1)) (block (; 1)))");
}

TEST(ParserTest, ParsesFunctionDeclaration) {
    // func add(x: int, y: int) -> int { return x + y; }
    std::vector<chtholly::Token> tokens = {
        chtholly::Token(chtholly::TokenType::FUNC, "func", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "add", 1),
        chtholly::Token(chtholly::TokenType::LEFT_PAREN, "(", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "x", 1),
        chtholly::Token(chtholly::TokenType::COLON, ":", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "int", 1),
        chtholly::Token(chtholly::TokenType::COMMA, ",", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "y", 1),
        chtholly::Token(chtholly::TokenType::COLON, ":", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "int", 1),
        chtholly::Token(chtholly::TokenType::RIGHT_PAREN, ")", 1),
        chtholly::Token(chtholly::TokenType::ARROW, "->", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "int", 1),
        chtholly::Token(chtholly::TokenType::LEFT_BRACE, "{", 1),
        chtholly::Token(chtholly::TokenType::RETURN, "return", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "x", 1),
        chtholly::Token(chtholly::TokenType::PLUS, "+", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "y", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::RIGHT_BRACE, "}", 1),
        chtholly::Token(chtholly::TokenType::END_OF_FILE, "", 1)
    };
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    chtholly::AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(func add(x: int, y: int) -> int (block (return (+ x y))))");
}

TEST(ParserTest, ParsesVarDeclarationWithType) {
    // let a: int = 10;
    std::vector<chtholly::Token> tokens = {
        chtholly::Token(chtholly::TokenType::LET, "let", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "a", 1),
        chtholly::Token(chtholly::TokenType::COLON, ":", 1),
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "int", 1),
        chtholly::Token(chtholly::TokenType::EQUAL, "=", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "10", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::END_OF_FILE, "", 1)
    };
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    chtholly::AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(var a: int = 10)");
}

TEST(ParserTest, ParsesCallExpression) {
    // add(1, 2);
    std::vector<chtholly::Token> tokens = {
        chtholly::Token(chtholly::TokenType::IDENTIFIER, "add", 1),
        chtholly::Token(chtholly::TokenType::LEFT_PAREN, "(", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "1", 1),
        chtholly::Token(chtholly::TokenType::COMMA, ",", 1),
        chtholly::Token(chtholly::TokenType::NUMBER, "2", 1),
        chtholly::Token(chtholly::TokenType::RIGHT_PAREN, ")", 1),
        chtholly::Token(chtholly::TokenType::SEMICOLON, ";", 1),
        chtholly::Token(chtholly::TokenType::END_OF_FILE, "", 1)
    };
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    ASSERT_EQ(statements.size(), 1);

    chtholly::AstPrinter printer;
    std::string result = printer.print(*statements[0]);
    EXPECT_EQ(result, "(; (call add 1 2))");
}
