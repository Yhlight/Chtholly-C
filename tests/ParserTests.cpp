#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"

using namespace chtholly;

TEST(ParserTest, SimpleVariableDeclaration) {
    std::string source = "let a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->getStatements().size(), 1);

    auto* varDecl = dynamic_cast<VarDeclAST*>(ast->getStatements()[0].get());
    ASSERT_NE(varDecl, nullptr);
    EXPECT_EQ(varDecl->getName(), "a");
    EXPECT_FALSE(varDecl->getIsMutable());

    auto* numLiteral = dynamic_cast<const NumberExprAST*>(varDecl->getInit());
    ASSERT_NE(numLiteral, nullptr);
}

TEST(ParserTest, MutableVariableDeclaration) {
    std::string source = "mut b = 20;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();

    ASSERT_NE(ast, nullptr);
    ASSERT_EQ(ast->getStatements().size(), 1);

    auto* varDecl = dynamic_cast<VarDeclAST*>(ast->getStatements()[0].get());
    ASSERT_NE(varDecl, nullptr);
    EXPECT_EQ(varDecl->getName(), "b");
    EXPECT_TRUE(varDecl->getIsMutable());

    auto* numLiteral = dynamic_cast<const NumberExprAST*>(varDecl->getInit());
    ASSERT_NE(numLiteral, nullptr);
}
