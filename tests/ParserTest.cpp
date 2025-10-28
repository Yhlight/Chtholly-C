#include <gtest/gtest.h>
#include "../src/Parser.h"
#include "../src/Lexer.h"
#include "../src/AST/AST.h"

using namespace Chtholly;

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
