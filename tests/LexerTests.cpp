#include <gtest/gtest.h>
#include "Lexer.h"
#include "TokenType.h"
#include <vector>

TEST(LexerTest, SingleCharacterTokens) {
    std::string source = "(){}[],.-+;*/:&?";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expectedTypes = {
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::LEFT_BRACKET, TokenType::RIGHT_BRACKET, TokenType::COMMA, TokenType::DOT,
        TokenType::MINUS, TokenType::PLUS, TokenType::SEMICOLON, TokenType::STAR, TokenType::SLASH,
        TokenType::COLON, TokenType::AMPERSAND, TokenType::QUESTION, TokenType::EOF_TOKEN
    };

    ASSERT_EQ(tokens.size(), expectedTypes.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expectedTypes[i]);
    }
}

TEST(LexerTest, OneOrTwoCharacterTokens) {
    std::string source = "! != = == > >= < <=";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expectedTypes = {
        TokenType::BANG, TokenType::BANG_EQUAL, TokenType::EQUAL, TokenType::EQUAL_EQUAL,
        TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL,
        TokenType::EOF_TOKEN
    };

    ASSERT_EQ(tokens.size(), expectedTypes.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expectedTypes[i]);
    }
}

TEST(LexerTest, Literals) {
    std::string source = "\"hello\" 123 45.67";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expectedTypes = {
        TokenType::STRING, TokenType::INTEGER, TokenType::DOUBLE, TokenType::EOF_TOKEN
    };

    ASSERT_EQ(tokens.size(), expectedTypes.size());
    EXPECT_EQ(tokens[0].type, expectedTypes[0]);
    EXPECT_EQ(std::any_cast<std::string>(tokens[0].literal), "hello");
    EXPECT_EQ(tokens[1].type, expectedTypes[1]);
    EXPECT_EQ(std::any_cast<int>(tokens[1].literal), 123);
    EXPECT_EQ(tokens[2].type, expectedTypes[2]);
    EXPECT_EQ(std::any_cast<double>(tokens[2].literal), 45.67);
}

TEST(LexerTest, IdentifiersAndKeywords) {
    std::string source = "let x = 10; func add() {}";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expectedTypes = {
        TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL, TokenType::INTEGER,
        TokenType::SEMICOLON, TokenType::FUNC, TokenType::IDENTIFIER,
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN, TokenType::LEFT_BRACE,
        TokenType::RIGHT_BRACE, TokenType::EOF_TOKEN
    };

    ASSERT_EQ(tokens.size(), expectedTypes.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expectedTypes[i]);
    }
}

TEST(LexerTest, Comments) {
    std::string source = "// this is a comment\nlet x = 10; /* another comment */";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expectedTypes = {
        TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL, TokenType::INTEGER,
        TokenType::SEMICOLON, TokenType::EOF_TOKEN
    };

    ASSERT_EQ(tokens.size(), expectedTypes.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expectedTypes[i]);
    }
}
