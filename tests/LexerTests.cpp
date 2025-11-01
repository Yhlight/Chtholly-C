#include <gtest/gtest.h>
#include "Lexer.h"
#include "Token.h"
#include <vector>

TEST(LexerTest, SingleCharacterTokens) {
    std::string source = "(){},.-+;*/:?";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::COMMA, TokenType::DOT, TokenType::MINUS, TokenType::PLUS, TokenType::SEMICOLON,
        TokenType::STAR, TokenType::SLASH, TokenType::COLON, TokenType::QUESTION, TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]);
    }
}

TEST(LexerTest, Operators) {
    std::string source = "! != = == > >= < <=";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::BANG, TokenType::BANG_EQUAL, TokenType::EQUAL, TokenType::EQUAL_EQUAL,
        TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL,
        TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]);
    }
}

TEST(LexerTest, Keywords) {
    std::string source = "let mut func if else return true false";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::LET, TokenType::MUT, TokenType::FUNC, TokenType::IF, TokenType::ELSE,
        TokenType::RETURN, TokenType::TRUE, TokenType::FALSE, TokenType::END_OF_FILE
    };

    ASSERT_EQ(tokens.size(), expected.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected[i]);
    }
}

TEST(LexerTest, Literals) {
    std::string source = "\"hello\" 123 45.67 var_name";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(std::get<std::string>(tokens[0].literal), "hello");

    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(std::get<double>(tokens[1].literal), 123);

    EXPECT_EQ(tokens[2].type, TokenType::NUMBER);
    EXPECT_EQ(std::get<double>(tokens[2].literal), 45.67);

    EXPECT_EQ(tokens[3].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[3].lexeme, "var_name");

    EXPECT_EQ(tokens[4].type, TokenType::END_OF_FILE);
}
