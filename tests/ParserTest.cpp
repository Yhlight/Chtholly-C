#include "../src/Parser.h"
#include <cassert>
#include <iostream>

void test_simple_variable_declaration() {
    std::string source = "let a = 10;";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    assert(program);
    assert(program->statements.size() == 1);

    auto stmt = dynamic_cast<Chtholly::VariableDeclaration*>(program->statements[0].get());
    assert(stmt);
    assert(stmt->token.type == Chtholly::TokenType::Let);
    assert(stmt->name->token.literal == "a");

    auto literal = dynamic_cast<Chtholly::IntegerLiteral*>(stmt->value.get());
    assert(literal);
    assert(literal->value == 10);
}

void test_binary_expression() {
    std::string source = "let x = 5 + 5 * 2;";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    assert(program);
    assert(program->statements.size() == 1);

    auto stmt = dynamic_cast<Chtholly::VariableDeclaration*>(program->statements[0].get());
    assert(stmt);

    auto binaryExpr = dynamic_cast<Chtholly::BinaryExpression*>(stmt->value.get());
    assert(binaryExpr);
    assert(binaryExpr->op.type == Chtholly::TokenType::Plus);

    auto left_literal = dynamic_cast<Chtholly::IntegerLiteral*>(binaryExpr->left.get());
    assert(left_literal);
    assert(left_literal->value == 5);

    auto right_binaryExpr = dynamic_cast<Chtholly::BinaryExpression*>(binaryExpr->right.get());
    assert(right_binaryExpr);
    assert(right_binaryExpr->op.type == Chtholly::TokenType::Asterisk);
}

int main() {
    test_simple_variable_declaration();
    test_binary_expression();
    return 0;
}
