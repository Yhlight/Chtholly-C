#include <gtest/gtest.h>
#include "Lexer.h"

using namespace chtholly;

TEST(LexerTest, SimpleVariableDeclaration) {
    std::string source = "let a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, TokenType::Let);
    EXPECT_EQ(tokens[1].type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].lexeme, "a");
    EXPECT_EQ(tokens[2].type, TokenType::Equal);
    EXPECT_EQ(tokens[3].type, TokenType::Integer);
    EXPECT_EQ(std::get<int>(tokens[3].literal), 10);
    EXPECT_EQ(tokens[4].type, TokenType::Semicolon);
    EXPECT_EQ(tokens[5].type, TokenType::EndOfFile);
}

TEST(LexerTest, AllOperators) {
    std::string source = "+ - * / % ++ -- += -= *= /= %= = == ! != < <= > >= & && | || -> => ? :";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 29);
    EXPECT_EQ(tokens[0].type, TokenType::Plus);
    EXPECT_EQ(tokens[1].type, TokenType::Minus);
    EXPECT_EQ(tokens[2].type, TokenType::Star);
    EXPECT_EQ(tokens[3].type, TokenType::Slash);
    EXPECT_EQ(tokens[4].type, TokenType::Mod);
    EXPECT_EQ(tokens[5].type, TokenType::PlusPlus);
    EXPECT_EQ(tokens[6].type, TokenType::MinusMinus);
    EXPECT_EQ(tokens[7].type, TokenType::PlusEqual);
    EXPECT_EQ(tokens[8].type, TokenType::MinusEqual);
    EXPECT_EQ(tokens[9].type, TokenType::StarEqual);
    EXPECT_EQ(tokens[10].type, TokenType::SlashEqual);
    EXPECT_EQ(tokens[11].type, TokenType::ModEqual);
    EXPECT_EQ(tokens[12].type, TokenType::Equal);
    EXPECT_EQ(tokens[13].type, TokenType::EqualEqual);
    EXPECT_EQ(tokens[14].type, TokenType::Bang);
    EXPECT_EQ(tokens[15].type, TokenType::BangEqual);
    EXPECT_EQ(tokens[16].type, TokenType::Less);
    EXPECT_EQ(tokens[17].type, TokenType::LessEqual);
    EXPECT_EQ(tokens[18].type, TokenType::Greater);
    EXPECT_EQ(tokens[19].type, TokenType::GreaterEqual);
    EXPECT_EQ(tokens[20].type, TokenType::Amp);
    EXPECT_EQ(tokens[21].type, TokenType::AmpAmp);
    EXPECT_EQ(tokens[22].type, TokenType::Pipe);
    EXPECT_EQ(tokens[23].type, TokenType::PipePipe);
    EXPECT_EQ(tokens[24].type, TokenType::Arrow);
    EXPECT_EQ(tokens[25].type, TokenType::FatArrow);
    EXPECT_EQ(tokens[26].type, TokenType::Question);
    EXPECT_EQ(tokens[27].type, TokenType::Colon);
    EXPECT_EQ(tokens[28].type, TokenType::EndOfFile);
}

TEST(LexerTest, Keywords) {
    std::string source = "func let mut if else return struct import public private self impl trait true false none switch case fallthrough break";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 21);
    EXPECT_EQ(tokens[0].type, TokenType::Func);
    EXPECT_EQ(tokens[1].type, TokenType::Let);
    EXPECT_EQ(tokens[2].type, TokenType::Mut);
    EXPECT_EQ(tokens[3].type, TokenType::If);
    EXPECT_EQ(tokens[4].type, TokenType::Else);
    EXPECT_EQ(tokens[5].type, TokenType::Return);
    EXPECT_EQ(tokens[6].type, TokenType::Struct);
    EXPECT_EQ(tokens[7].type, TokenType::Import);
    EXPECT_EQ(tokens[8].type, TokenType::Public);
    EXPECT_EQ(tokens[9].type, TokenType::Private);
    EXPECT_EQ(tokens[10].type, TokenType::Self);
    EXPECT_EQ(tokens[11].type, TokenType::Impl);
    EXPECT_EQ(tokens[12].type, TokenType::Trait);
    EXPECT_EQ(tokens[13].type, TokenType::True);
    EXPECT_EQ(tokens[14].type, TokenType::False);
    EXPECT_EQ(tokens[15].type, TokenType::None);
    EXPECT_EQ(tokens[16].type, TokenType::Switch);
    EXPECT_EQ(tokens[17].type, TokenType::Case);
    EXPECT_EQ(tokens[18].type, TokenType::Fallthrough);
    EXPECT_EQ(tokens[19].type, TokenType::Break);
    EXPECT_EQ(tokens[20].type, TokenType::EndOfFile);
}

TEST(LexerTest, LiteralsAndComments) {
    std::string source = R"("hello" 123 45.67 // comment
    /* block */)";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::String);
    EXPECT_EQ(std::get<std::string>(tokens[0].literal), "hello");
    EXPECT_EQ(tokens[1].type, TokenType::Integer);
    EXPECT_EQ(std::get<int>(tokens[1].literal), 123);
    EXPECT_EQ(tokens[2].type, TokenType::Double);
    EXPECT_EQ(std::get<double>(tokens[2].literal), 45.67);
    EXPECT_EQ(tokens[3].type, TokenType::EndOfFile);
}
