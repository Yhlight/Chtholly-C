#include <gtest/gtest.h>
#include "Lexer.h"

TEST(LexerTest, HandlesEmptySource) {
    chtholly::Lexer lexer("");
    chtholly::Token token = lexer.scanToken();
    EXPECT_EQ(token.type, chtholly::TokenType::END_OF_FILE);
}

TEST(LexerTest, ScansSingleCharacterTokens) {
    chtholly::Lexer lexer("(){};,.-+/*");
    std::vector<chtholly::TokenType> expected_types = {
        chtholly::TokenType::LEFT_PAREN,
        chtholly::TokenType::RIGHT_PAREN,
        chtholly::TokenType::LEFT_BRACE,
        chtholly::TokenType::RIGHT_BRACE,
        chtholly::TokenType::SEMICOLON,
        chtholly::TokenType::COMMA,
        chtholly::TokenType::DOT,
        chtholly::TokenType::MINUS,
        chtholly::TokenType::PLUS,
        chtholly::TokenType::SLASH,
        chtholly::TokenType::STAR,
        chtholly::TokenType::END_OF_FILE,
    };

    for (const auto& expected_type : expected_types) {
        chtholly::Token token = lexer.scanToken();
        EXPECT_EQ(token.type, expected_type);
    }
}

TEST(LexerTest, ScansNumberLiterals) {
    chtholly::Lexer lexer("123 45.67");

    chtholly::Token int_token = lexer.scanToken();
    EXPECT_EQ(int_token.type, chtholly::TokenType::NUMBER);
    EXPECT_EQ(int_token.lexeme, "123");

    chtholly::Token float_token = lexer.scanToken();
    EXPECT_EQ(float_token.type, chtholly::TokenType::NUMBER);
    EXPECT_EQ(float_token.lexeme, "45.67");

    chtholly::Token eof_token = lexer.scanToken();
    EXPECT_EQ(eof_token.type, chtholly::TokenType::END_OF_FILE);
}

TEST(LexerTest, ScansIdentifiersAndKeywords) {
    chtholly::Lexer lexer("let my_var = func");

    chtholly::Token let_token = lexer.scanToken();
    EXPECT_EQ(let_token.type, chtholly::TokenType::LET);
    EXPECT_EQ(let_token.lexeme, "let");

    chtholly::Token identifier_token = lexer.scanToken();
    EXPECT_EQ(identifier_token.type, chtholly::TokenType::IDENTIFIER);
    EXPECT_EQ(identifier_token.lexeme, "my_var");

    chtholly::Token equal_token = lexer.scanToken();
    EXPECT_EQ(equal_token.type, chtholly::TokenType::EQUAL);

    chtholly::Token func_token = lexer.scanToken();
    EXPECT_EQ(func_token.type, chtholly::TokenType::FUNC);
    EXPECT_EQ(func_token.lexeme, "func");

    chtholly::Token eof_token = lexer.scanToken();
    EXPECT_EQ(eof_token.type, chtholly::TokenType::END_OF_FILE);
}

TEST(LexerTest, ScansBrackets) {
    chtholly::Lexer lexer("[]");

    chtholly::Token left_token = lexer.scanToken();
    EXPECT_EQ(left_token.type, chtholly::TokenType::LEFT_BRACKET);

    chtholly::Token right_token = lexer.scanToken();
    EXPECT_EQ(right_token.type, chtholly::TokenType::RIGHT_BRACKET);
}

TEST(LexerTest, ScansStringLiterals) {
    chtholly::Lexer lexer("\"hello world\"");

    chtholly::Token token = lexer.scanToken();
    EXPECT_EQ(token.type, chtholly::TokenType::STRING);
    EXPECT_EQ(token.lexeme, "\"hello world\"");

    chtholly::Token eof_token = lexer.scanToken();
    EXPECT_EQ(eof_token.type, chtholly::TokenType::END_OF_FILE);
}

TEST(LexerTest, ScansTwoCharacterTokens) {
    chtholly::Lexer lexer("!= == <= >= -> ::");
    std::vector<chtholly::TokenType> expected_types = {
        chtholly::TokenType::BANG_EQUAL,
        chtholly::TokenType::EQUAL_EQUAL,
        chtholly::TokenType::LESS_EQUAL,
        chtholly::TokenType::GREATER_EQUAL,
        chtholly::TokenType::ARROW,
        chtholly::TokenType::COLON_COLON,
        chtholly::TokenType::END_OF_FILE,
    };

    for (const auto& expected_type : expected_types) {
        chtholly::Token token = lexer.scanToken();
        EXPECT_EQ(token.type, expected_type);
    }
}
