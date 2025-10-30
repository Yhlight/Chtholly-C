#include <gtest/gtest.h>
#include "../src/Lexer.h"
#include "../src/Token.h"

#include <vector>

TEST(LexerTest, BasicTokens) {
    std::string source = "let x = 10; () {} []";
    Lexer lexer(source);
    std::vector<TokenType> expected_tokens = {
        TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL,
        TokenType::NUMBER_INT, TokenType::SEMICOLON,
        TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN,
        TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE,
        TokenType::LEFT_BRACKET, TokenType::RIGHT_BRACKET,
        TokenType::END_OF_FILE
    };

    for (const auto& expected_token : expected_tokens) {
        Token token = lexer.nextToken();
        EXPECT_EQ(token.type, expected_token);
    }
}

TEST(LexerTest, Operators) {
    std::string source = "+ - * / % = == != < <= > >= && || ! &";
    Lexer lexer(source);
    std::vector<TokenType> expected_tokens = {
        TokenType::PLUS, TokenType::MINUS, TokenType::STAR, TokenType::SLASH, TokenType::MODULO,
        TokenType::EQUAL, TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL,
        TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL,
        TokenType::LOGICAL_AND, TokenType::LOGICAL_OR, TokenType::BANG, TokenType::AMPERSAND,
        TokenType::END_OF_FILE
    };

    for (const auto& expected_token : expected_tokens) {
        Token token = lexer.nextToken();
        EXPECT_EQ(token.type, expected_token);
    }
}

TEST(LexerTest, Keywords) {
    std::string source = "struct func let mut if else return true false switch case fallthrough break public private self impl trait import none";
    Lexer lexer(source);
    std::vector<TokenType> expected_tokens = {
        TokenType::STRUCT, TokenType::FUNC, TokenType::LET, TokenType::MUT,
        TokenType::IF, TokenType::ELSE, TokenType::RETURN, TokenType::TRUE, TokenType::FALSE,
        TokenType::SWITCH, TokenType::CASE, TokenType::FALLTHROUGH, TokenType::BREAK,
        TokenType::PUBLIC, TokenType::PRIVATE, TokenType::SELF, TokenType::IMPL, TokenType::TRAIT,
        TokenType::IMPORT, TokenType::NONE,
        TokenType::END_OF_FILE
    };

    for (const auto& expected_token : expected_tokens) {
        Token token = lexer.nextToken();
        EXPECT_EQ(token.type, expected_token);
    }
}

TEST(LexerTest, Literals) {
    std::string source = R"("hello" 123 45.67 'a')";
    Lexer lexer(source);

    Token token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::STRING);
    EXPECT_EQ(token.lexeme, "\"hello\"");

    token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::NUMBER_INT);
    EXPECT_EQ(token.lexeme, "123");

    token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::NUMBER_FLOAT);
    EXPECT_EQ(token.lexeme, "45.67");

    token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::CHAR);
    EXPECT_EQ(token.lexeme, "'a'");

    token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::END_OF_FILE);
}

TEST(LexerTest, Comments) {
    std::string source = R"(
        // this is a comment
        let x = 10; // another comment
        /*
         * a block comment
         */
        let y = 20;
    )";
    Lexer lexer(source);
    std::vector<TokenType> expected_tokens = {
        TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL, TokenType::NUMBER_INT, TokenType::SEMICOLON,
        TokenType::LET, TokenType::IDENTIFIER, TokenType::EQUAL, TokenType::NUMBER_INT, TokenType::SEMICOLON,
        TokenType::END_OF_FILE
    };

    for (const auto& expected_token : expected_tokens) {
        Token token = lexer.nextToken();
        EXPECT_EQ(token.type, expected_token);
    }
}
