#include <gtest/gtest.h>
#include "Resolver.h"
#include "Parser.h"
#include "Lexer.h"
#include "Error.h"
#include <vector>
#include <memory>

static void resolveSource(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
}

struct ResolverTest : testing::Test {
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(ResolverTest, RedeclareVariableInSameScope) {
    resolveSource("{ let a = 1; let a = 2; }");
    EXPECT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, UseVariableInOwnInitializer) {
    resolveSource("{ let a = a; }");
    EXPECT_TRUE(ErrorReporter::hadError);
}

TEST_F(ResolverTest, AllowsShadowing) {
    resolveSource("{ let a = 1; { let a = 2; } }");
    EXPECT_FALSE(ErrorReporter::hadError);
}
