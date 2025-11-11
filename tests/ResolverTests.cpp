#include <gtest/gtest.h>
#include "Resolver.h"
#include "Parser.h"
#include "Lexer.h"
#include <vector>
#include <string>

using namespace chtholly;

class ResolverTest : public ::testing::Test {
protected:
    bool resolve(const std::string& source) {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();
        Parser parser(tokens);
        auto statements = parser.parse();
        Resolver resolver;
        resolver.resolve(statements);
        return resolver.hadError;
    }
};

TEST_F(ResolverTest, ValidBlock) {
    std::string source = "{ let a = 1; let b = 2; }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, RedeclareVariableInSameScope) {
    std::string source = "{ let a = 1; let a = 2; }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ReadVariableInOwnInitializer) {
    std::string source = "{ let a = a; }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, UseUndefinedVariable) {
    std::string source = "{ print(a); }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, AssignToUndefinedVariable) {
    std::string source = "{ a = 1; }";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, Shadowing) {
    std::string source = "{ let a = 1; { let a = 2; print(a); } print(a); }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ValidFunction) {
    std::string source = "func foo() { return 1; }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ReturnAtTopLevel) {
    std::string source = "return 1;";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidSelf) {
    std::string source = "struct Foo { func bar() { print(self); } }";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, SelfOutsideOfClass) {
    std::string source = "print(self);";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidTypeInference) {
    std::string source = "let a = 1;";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, ValidAssignment) {
    std::string source = "let a: int = 1; a = 2;";
    ASSERT_FALSE(resolve(source));
}

TEST_F(ResolverTest, TypeMismatchInAssignment) {
    std::string source = "let a: int = 1; a = \"hello\";";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, InvalidOperandsForBinary) {
    std::string source = "let a = true + false;";
    ASSERT_TRUE(resolve(source));
}

// Function Call Tests
TEST_F(ResolverTest, CallNonFunction) {
    std::string source = "let a = 1; a();";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, TooManyArguments) {
    std::string source = "input(1);";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, TooFewArguments) {
    std::string source = "fs_read();";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, IncorrectArgumentType) {
    std::string source = "fs_write(1, 2);";
    ASSERT_TRUE(resolve(source));
}

TEST_F(ResolverTest, ValidFunctionCall) {
    std::string source = "let s = input();";
    ASSERT_FALSE(resolve(source));
}
