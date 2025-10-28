#include <gtest/gtest.h>
#include "../src/Lexer.h"

using namespace Chtholly;

TEST(LexerTest, TokenizeFuncKeyword) {
    Lexer lexer("func");
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::Func);
    EXPECT_EQ(tokens[0].text, "func");
    EXPECT_EQ(tokens[1].type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizeInteger) {
    Lexer lexer("123");
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::Integer);
    EXPECT_EQ(tokens[0].text, "123");
    EXPECT_EQ(std::get<long long>(tokens[0].value), 123);
    EXPECT_EQ(tokens[1].type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizeOperators) {
    Lexer lexer("+-*/% = == != < <= > >= && || ! & |");
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 18);
    EXPECT_EQ(tokens[0].type, TokenType::Plus);
    EXPECT_EQ(tokens[1].type, TokenType::Minus);
    EXPECT_EQ(tokens[2].type, TokenType::Star);
    EXPECT_EQ(tokens[3].type, TokenType::Slash);
    EXPECT_EQ(tokens[4].type, TokenType::Mod);
    EXPECT_EQ(tokens[5].type, TokenType::Assign);
    EXPECT_EQ(tokens[6].type, TokenType::Equal);
    EXPECT_EQ(tokens[7].type, TokenType::NotEqual);
    EXPECT_EQ(tokens[8].type, TokenType::Less);
    EXPECT_EQ(tokens[9].type, TokenType::LessEqual);
    EXPECT_EQ(tokens[10].type, TokenType::Greater);
    EXPECT_EQ(tokens[11].type, TokenType::GreaterEqual);
    EXPECT_EQ(tokens[12].type, TokenType::LogicalAnd);
    EXPECT_EQ(tokens[13].type, TokenType::LogicalOr);
    EXPECT_EQ(tokens[14].type, TokenType::LogicalNot);
    EXPECT_EQ(tokens[15].type, TokenType::Ampersand);
    EXPECT_EQ(tokens[16].type, TokenType::BitwiseOr);
    EXPECT_EQ(tokens[17].type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizeString) {
    Lexer lexer("\"hello world\"");
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::String);
    EXPECT_EQ(tokens[0].text, "\"hello world\"");
    EXPECT_EQ(std::get<std::string>(tokens[0].value), "hello world");
    EXPECT_EQ(tokens[1].type, TokenType::EndOfFile);
}

TEST(LexerTest, TokenizeChar) {
    Lexer lexer("'a'");
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::Char);
    EXPECT_EQ(tokens[0].text, "'a'");
    EXPECT_EQ(std::get<char>(tokens[0].value), 'a');
    EXPECT_EQ(tokens[1].type, TokenType::EndOfFile);
}
