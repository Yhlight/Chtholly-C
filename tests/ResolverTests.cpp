#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"

TEST(ResolverTest, ValidResolution) {
    std::string source = "let a = 1; let b = 2; { let c = a + b; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    ASSERT_NO_THROW(resolver.resolve(statements));
}

TEST(ResolverTest, UndeclaredVariable) {
    std::string source = "a = 1;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    ASSERT_THROW(resolver.resolve(statements), std::runtime_error);
}

TEST(ResolverTest, RedeclaredVariable) {
    std::string source = "let a = 1; let a = 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    ASSERT_THROW(resolver.resolve(statements), std::runtime_error);
}

TEST(ResolverTest, Shadowing) {
    std::string source = "let a = 1; { let a = 2; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    ASSERT_NO_THROW(resolver.resolve(statements));
}

TEST(ResolverTest, ValidMutableAssignment) {
    std::string source = "mut a = 1; a = 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    ASSERT_NO_THROW(resolver.resolve(statements));
}

TEST(ResolverTest, InvalidImmutableAssignment) {
    std::string source = "let a = 1; a = 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    ASSERT_THROW(resolver.resolve(statements), std::runtime_error);
}
