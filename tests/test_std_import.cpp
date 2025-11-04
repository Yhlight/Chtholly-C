#include "gtest/gtest.h"
#include "../src/frontend/Parser.h"
#include "../src/frontend/Lexer.h"
#include "../src/frontend/AST/ASTPrinter.h"
#include "../src/frontend/AST/Stmt.h"

TEST(ParserTest, StdImport) {
    std::string source = "import iostream;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    ImportStmt* importStmt = dynamic_cast<ImportStmt*>(statements[0].get());
    ASSERT_NE(importStmt, nullptr);
    EXPECT_EQ(importStmt->path.lexeme, "iostream");
    EXPECT_TRUE(importStmt->is_std);
}
