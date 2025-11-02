#include <gtest/gtest.h>
#include "../src/Resolver.h"
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include "../src/Error.h"

class ResolverTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::hadError = false;
    }

    void TearDown() override {
        ErrorReporter::hadError = false;
    }
};

TEST_F(ResolverTest, RedeclareVariable) {
    std::string source = "{ let a = 1; let a = 2; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, ReadInInitializer) {
    std::string source = "{ let a = a; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}
