#include <gtest/gtest.h>
#include "Parser.h"
#include "AST.h"
#include <vector>

TEST(Parser, TestIdentifierExpression) {
    std::string input = "foobar;";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<Chtholly::ExpressionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto ident = dynamic_cast<Chtholly::Identifier*>(stmt->expression.get());
    ASSERT_NE(ident, nullptr);
    ASSERT_EQ(ident->value, "foobar");
    ASSERT_EQ(ident->tokenLiteral(), "foobar");
}

TEST(Parser, TestIntegerLiteralExpression) {
    std::string input = "5;";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<Chtholly::ExpressionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto literal = dynamic_cast<Chtholly::IntegerLiteral*>(stmt->expression.get());
    ASSERT_NE(literal, nullptr);
    ASSERT_EQ(literal->value, 5);
    ASSERT_EQ(literal->tokenLiteral(), "5");
}

TEST(Parser, TestParsingPrefixExpressions) {
    std::string input = "!5;";
    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<Chtholly::ExpressionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto exp = dynamic_cast<Chtholly::PrefixExpression*>(stmt->expression.get());
    ASSERT_NE(exp, nullptr);
    ASSERT_EQ(exp->op, "!");

    auto literal = dynamic_cast<Chtholly::IntegerLiteral*>(exp->right.get());
    ASSERT_NE(literal, nullptr);
    ASSERT_EQ(literal->value, 5);
}

TEST(Parser, TestParsingInfixExpressions) {
    std::string input = "5 + 5;";
    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<Chtholly::ExpressionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto exp = dynamic_cast<Chtholly::InfixExpression*>(stmt->expression.get());
    ASSERT_NE(exp, nullptr);
    ASSERT_EQ(exp->op, "+");

    auto left = dynamic_cast<Chtholly::IntegerLiteral*>(exp->left.get());
    ASSERT_NE(left, nullptr);
    ASSERT_EQ(left->value, 5);

    auto right = dynamic_cast<Chtholly::IntegerLiteral*>(exp->right.get());
    ASSERT_NE(right, nullptr);
    ASSERT_EQ(right->value, 5);
}
