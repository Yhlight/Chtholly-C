#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"
#include "Error.h"

class ResolverTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(ResolverTest, ValidResolution) {
    std::string source = "let a = 1; let b = 2; { let c = a + b; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    resolver.resolve(statements);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, UndeclaredVariable) {
    std::string source = "a = 1;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    resolver.resolve(statements);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, RedeclaredVariable) {
    std::string source = "let a = 1; let a = 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    resolver.resolve(statements);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, Shadowing) {
    std::string source = "let a = 1; { let a = 2; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    resolver.resolve(statements);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, ValidMutableAssignment) {
    std::string source = "mut a = 1; a = 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    resolver.resolve(statements);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, InvalidImmutableAssignment) {
    std::string source = "let a = 1; a = 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    resolver.resolve(statements);
    ASSERT_TRUE(ErrorReporter::hadError);
}
