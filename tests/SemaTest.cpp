#include <gtest/gtest.h>
#include "Sema.h"
#include "Parser.h"
#include "Lexer.h"
#include <vector>
#include <memory>

static std::vector<std::unique_ptr<chtholly::Stmt>> parse(const std::string& source) {
    chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    return parser.parse();
}

TEST(SemaTest, VarRedefinition) {
    std::string source = "let a = 10; let a = 20;";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_TRUE(sema.hadError());
}

TEST(SemaTest, UndefinedVariable) {
    std::string source = "let b = a;";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_TRUE(sema.hadError());
}

TEST(SemaTest, TypeInference) {
    std::string source = "let a = 10; let b = a;";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_FALSE(sema.hadError());
}

TEST(SemaTest, BinaryExprTypeCheck) {
    std::string source = "let a = 10 + 20;";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_FALSE(sema.hadError());
}

TEST(SemaTest, BinaryExprTypeError) {
    std::string source = "let a = 10 + \"hello\";";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_TRUE(sema.hadError());
}
