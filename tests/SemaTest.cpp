#include <gtest/gtest.h>
#include "Sema.h"
#include "Parser.h"
#include "Lexer.h"

using namespace chtholly;

// Helper function to parse a source string and run semantic analysis
Sema analyzeSource(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Sema sema;
    sema.analyze(stmts);
    return sema;
}

TEST(SemaTest, VarDeclSuccess) {
    Sema sema = analyzeSource("let a = 10;");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, VarRedefinitionError) {
    Sema sema = analyzeSource("let a = 10; let a = 20;");
    EXPECT_TRUE(sema.hadError());
}

TEST(SemaTest, VarDeclWithTypeAnnotation) {
    Sema sema = analyzeSource("let a: int = 10;");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, VarInferenceFromLiteral) {
    Sema sema = analyzeSource("let a = \"hello\";");
    EXPECT_FALSE(sema.hadError());
}

TEST(SemaTest, UndeclaredVariable) {
    Sema sema = analyzeSource("let a = b;");
    EXPECT_TRUE(sema.hadError());
}
