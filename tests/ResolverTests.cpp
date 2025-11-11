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
