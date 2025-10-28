#include <gtest/gtest.h>
#include "../src/Parser.h"
#include "../src/Lexer.h"
#include "../src/AST/AST.h"
#include "../src/AST/ASTPrinter.h"
#include <vector>
#include <memory>

using namespace Chtholly;

static std::string getAST(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    ASTPrinter printer;
    return printer.print(statements);
}

TEST(ParserTest, ParseVarDeclaration) {
    std::string source = "let a = 123;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    VarDeclStmt* varDecl = dynamic_cast<VarDeclStmt*>(statements[0].get());
    ASSERT_NE(varDecl, nullptr);
    EXPECT_EQ(varDecl->name.text, "a");
    NumberLiteralExpr* number = dynamic_cast<NumberLiteralExpr*>(varDecl->initializer.get());
    ASSERT_NE(number, nullptr);
    EXPECT_EQ(number->number.text, "123");
}

TEST(ParserTest, ParseExpression) {
    std::string source = "1 + 2 * 3;";
    std::string expected = "(expr (+ 1 (* 2 3)))\n";
    EXPECT_EQ(getAST(source), expected);

    source = "(1 + 2) * 3;";
    expected = "(expr (* (group (+ 1 2)) 3))\n";
    EXPECT_EQ(getAST(source), expected);

    source = "-1 + 2;";
    expected = "(expr (+ (- 1) 2))\n";
    EXPECT_EQ(getAST(source), expected);

    source = "a = 1 + 2;";
    expected = "(expr (a = (+ 1 2)))\n";
    EXPECT_EQ(getAST(source), expected);
}
