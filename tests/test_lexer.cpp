#include <gtest/gtest.h>
#include "Lexer.h"
#include <vector>

TEST(Lexer, TestNextToken_Comprehensive) {
    std::string input = R"(
        let five = 5;
        let ten = 10.5;
        let add = func(x, y) {
            return x + y;
        };
        let result = add(five, ten);

        if (result != 15.5) {
            return false;
        } else {
            return true;
        }

        "hello"
        'c'
    )";

    Chtholly::Lexer l(input);

    std::vector<Chtholly::Token> expected_tokens = {
        {Chtholly::TokenType::Let, "let", 2, 9},
        {Chtholly::TokenType::Identifier, "five", 2, 13},
        {Chtholly::TokenType::Assign, "=", 2, 18},
        {Chtholly::TokenType::Int, "5", 2, 20},
        {Chtholly::TokenType::Semicolon, ";", 2, 21},
        {Chtholly::TokenType::Let, "let", 3, 9},
        {Chtholly::TokenType::Identifier, "ten", 3, 13},
        {Chtholly::TokenType::Assign, "=", 3, 17},
        {Chtholly::TokenType::Double, "10.5", 3, 19},
        {Chtholly::TokenType::Semicolon, ";", 3, 23},
        {Chtholly::TokenType::Let, "let", 4, 9},
        {Chtholly::TokenType::Identifier, "add", 4, 13},
        {Chtholly::TokenType::Assign, "=", 4, 17},
        {Chtholly::TokenType::Func, "func", 4, 19},
        {Chtholly::TokenType::LParen, "(", 4, 23},
        {Chtholly::TokenType::Identifier, "x", 4, 24},
        {Chtholly::TokenType::Comma, ",", 4, 25},
        {Chtholly::TokenType::Identifier, "y", 4, 27},
        {Chtholly::TokenType::RParen, ")", 4, 28},
        {Chtholly::TokenType::LBrace, "{", 4, 30},
        {Chtholly::TokenType::Return, "return", 5, 13},
        {Chtholly::TokenType::Identifier, "x", 5, 20},
        {Chtholly::TokenType::Plus, "+", 5, 22},
        {Chtholly::TokenType::Identifier, "y", 5, 24},
        {Chtholly::TokenType::Semicolon, ";", 5, 25},
        {Chtholly::TokenType::RBrace, "}", 6, 9},
        {Chtholly::TokenType::Semicolon, ";", 6, 10},
        {Chtholly::TokenType::Let, "let", 7, 9},
        {Chtholly::TokenType::Identifier, "result", 7, 13},
        {Chtholly::TokenType::Assign, "=", 7, 20},
        {Chtholly::TokenType::Identifier, "add", 7, 22},
        {Chtholly::TokenType::LParen, "(", 7, 25},
        {Chtholly::TokenType::Identifier, "five", 7, 26},
        {Chtholly::TokenType::Comma, ",", 7, 30},
        {Chtholly::TokenType::Identifier, "ten", 7, 32},
        {Chtholly::TokenType::RParen, ")", 7, 35},
        {Chtholly::TokenType::Semicolon, ";", 7, 36},
        {Chtholly::TokenType::If, "if", 9, 9},
        {Chtholly::TokenType::LParen, "(", 9, 12},
        {Chtholly::TokenType::Identifier, "result", 9, 13},
        {Chtholly::TokenType::NotEqual, "!=", 9, 20},
        {Chtholly::TokenType::Double, "15.5", 9, 23},
        {Chtholly::TokenType::RParen, ")", 9, 27},
        {Chtholly::TokenType::LBrace, "{", 9, 29},
        {Chtholly::TokenType::Return, "return", 10, 13},
        {Chtholly::TokenType::False, "false", 10, 20},
        {Chtholly::TokenType::Semicolon, ";", 10, 25},
        {Chtholly::TokenType::RBrace, "}", 11, 9},
        {Chtholly::TokenType::Else, "else", 11, 11},
        {Chtholly::TokenType::LBrace, "{", 11, 16},
        {Chtholly::TokenType::Return, "return", 12, 13},
        {Chtholly::TokenType::True, "true", 12, 20},
        {Chtholly::TokenType::Semicolon, ";", 12, 24},
        {Chtholly::TokenType::RBrace, "}", 13, 9},
        {Chtholly::TokenType::String, "hello", 15, 9},
        {Chtholly::TokenType::Char, "c", 16, 9},
        {Chtholly::TokenType::Eof, "", 17, 5},
    };

    for (const auto& expected_tok : expected_tokens) {
        Chtholly::Token tok = l.nextToken();
        ASSERT_EQ(tok.type, expected_tok.type);
        ASSERT_EQ(tok.literal, expected_tok.literal);
        ASSERT_EQ(tok.line, expected_tok.line);
        ASSERT_EQ(tok.col, expected_tok.col);
    }
}
