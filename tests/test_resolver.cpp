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

TEST_F(ResolverTest, PrivateFieldAccess) {
    std::string source = "struct Point { private let x: int; } let p = Point{x: 1}; print(p.x);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, TypeInference) {
    std::string source = "let a = 1.0 + 2.0;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_FALSE(ErrorReporter::hadError);
    auto* letStmt = dynamic_cast<LetStmt*>(stmts[0].get());
    ASSERT_NE(letStmt, nullptr);
    ASSERT_TRUE(letStmt->initializer->resolved_type.has_value());
    ASSERT_EQ(letStmt->initializer->resolved_type->baseType.lexeme, "double");
}

TEST_F(ResolverTest, UseOfMovedValue) {
    std::string source = "mut x = 1; let y = &mut x; print(x);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, MutableBorrowOfImmutablyBorrowedVariable) {
    std::string source = "mut x = 1; let y = &x; let z = &mut x;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, MutableBorrowOfImmutableVariable) {
    std::string source = "let x = 1; let y = &mut x;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, ReturnOutsideFunction) {
    std::string source = "return 1;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, IfScope) {
    std::string source = "if (true) { let a = 1; } print(a);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, WhileScope) {
    std::string source = "while (true) { let a = 1; } print(a);";
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
