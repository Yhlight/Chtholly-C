#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"

TEST(SemanticAnalyzerTest, DetectsRedeclaredVariable) {
    std::string source = "let x = 1; let x = 2;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.tokenize();
    Chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<Chtholly::Stmt>> statements = parser.parse();

    ASSERT_FALSE(parser.hadError);

    Chtholly::SemanticAnalyzer analyzer;
    analyzer.analyze(statements);
    ASSERT_TRUE(analyzer.hadError);
}

TEST(SemanticAnalyzerTest, DetectsMismatchedTypes) {
    std::string source = "let x: int = true;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.tokenize();
    Chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<Chtholly::Stmt>> statements = parser.parse();

    ASSERT_FALSE(parser.hadError);

    Chtholly::SemanticAnalyzer analyzer;
    analyzer.analyze(statements);
    ASSERT_TRUE(analyzer.hadError);
}

TEST(SemanticAnalyzerTest, DetectsMismatchedTypesInBinaryExpr) {
    std::string source = "let x = 1 + true;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.tokenize();
    Chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<Chtholly::Stmt>> statements = parser.parse();

    ASSERT_FALSE(parser.hadError);

    Chtholly::SemanticAnalyzer analyzer;
    analyzer.analyze(statements);
    ASSERT_TRUE(analyzer.hadError);
}
