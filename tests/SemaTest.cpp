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

TEST(SemaTest, Assignment) {
    std::string source = "mut a = 10; a = 20;";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_FALSE(sema.hadError());
}

TEST(SemaTest, AssignmentToImmutable) {
    std::string source = "let a = 10; a = 20;";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_TRUE(sema.hadError());
}

TEST(SemaTest, AssignmentTypeError) {
    std::string source = "mut a = 10; a = \"hello\";";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_TRUE(sema.hadError());
}

TEST(SemaTest, IfStatement) {
    std::string source = "if (true) { let a = 10; }";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_FALSE(sema.hadError());
}

TEST(SemaTest, IfStatementTypeError) {
    std::string source = "if (10) { let a = 10; }";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_TRUE(sema.hadError());
}

TEST(SemaTest, WhileStatement) {
    std::string source = "while (true) { let a = 10; }";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_FALSE(sema.hadError());
}

TEST(SemaTest, WhileStatementTypeError) {
    std::string source = "while (10) { let a = 10; }";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_TRUE(sema.hadError());
}

TEST(SemaTest, ArithmeticBinaryExpr) {
    std::string source = "let a = 10.0 + 5.0; let b = 10.0 - 5.0; let c = 10.0 * 5.0; let d = 10.0 / 5.0;";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_FALSE(sema.hadError());
}

TEST(SemaTest, ComparisonBinaryExpr) {
    std::string source = "let a = 10.0 > 5.0; let b = 10.0 >= 5.0; let c = 10.0 < 5.0; let d = 10.0 <= 5.0;";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_FALSE(sema.hadError());
}

TEST(SemaTest, EqualityBinaryExpr) {
    std::string source = "let a = 10.0 == 5.0; let b = 10.0 != 5.0; let c = true == false; let d = true != false;";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_FALSE(sema.hadError());
}

TEST(SemaTest, LogicalBinaryExpr) {
    std::string source = "let a = true and false; let b = true or false;";
    auto stmts = parse(source);
    chtholly::Sema sema;
    sema.analyze(stmts);
    ASSERT_FALSE(sema.hadError());
}
