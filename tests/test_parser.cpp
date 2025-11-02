#include "gtest/gtest.h"
#include "../src/Parser.h"
#include "../src/Lexer.h"
#include "../src/AST/ASTPrinter.h"
#include "../src/AST/Stmt.h"

TEST(ParserTest, SimpleExpression) {
    std::string source = "1 + 2 * 3;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
    ASSERT_NE(exprStmt, nullptr);
    ASTPrinter printer;
    std::string result = printer.print(*exprStmt->expression);
    ASSERT_EQ(result, "(+ 1 (* 2 3))");
}

TEST(ParserTest, GroupingExpression) {
    std::string source = "(1 + 2) * 3;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
    ASSERT_NE(exprStmt, nullptr);
    ASTPrinter printer;
    std::string result = printer.print(*exprStmt->expression);
    ASSERT_EQ(result, "(* (group (+ 1 2)) 3)");
}

TEST(ParserTest, LetStatement) {
    std::string source = "let x = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    LetStmt* letStmt = dynamic_cast<LetStmt*>(statements[0].get());
    ASSERT_NE(letStmt, nullptr);
    EXPECT_EQ(letStmt->name.lexeme, "x");
}

TEST(ParserTest, PrintStatement) {
    std::string source = "print 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    PrintStmt* printStmt = dynamic_cast<PrintStmt*>(statements[0].get());
    ASSERT_NE(printStmt, nullptr);
}

TEST(ParserTest, MutLetStatement) {
    std::string source = "mut x = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    LetStmt* letStmt = dynamic_cast<LetStmt*>(statements[0].get());
    ASSERT_NE(letStmt, nullptr);
    EXPECT_EQ(letStmt->name.lexeme, "x");
    EXPECT_TRUE(letStmt->isMutable);
}

TEST(ParserTest, AssignmentExpression) {
    std::string source = "x = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    ExpressionStmt* stmt = dynamic_cast<ExpressionStmt*>(statements[0].get());
    ASSERT_NE(stmt, nullptr);
    AssignExpr* expr = dynamic_cast<AssignExpr*>(stmt->expression.get());
    ASSERT_NE(expr, nullptr);
    EXPECT_EQ(expr->name.lexeme, "x");
}

TEST(ParserTest, IfStatement) {
    std::string source = "if (x) print 1;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    IfStmt* ifStmt = dynamic_cast<IfStmt*>(statements[0].get());
    ASSERT_NE(ifStmt, nullptr);
    EXPECT_NE(ifStmt->thenBranch, nullptr);
    EXPECT_EQ(ifStmt->elseBranch, nullptr);
}

TEST(ParserTest, IfElseStatement) {
    std::string source = "if (x) print 1; else print 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    IfStmt* ifStmt = dynamic_cast<IfStmt*>(statements[0].get());
    ASSERT_NE(ifStmt, nullptr);
    EXPECT_NE(ifStmt->thenBranch, nullptr);
    EXPECT_NE(ifStmt->elseBranch, nullptr);
}

TEST(ParserTest, WhileStatement) {
    std::string source = "while (x) print 1;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    WhileStmt* whileStmt = dynamic_cast<WhileStmt*>(statements[0].get());
    ASSERT_NE(whileStmt, nullptr);
    EXPECT_NE(whileStmt->body, nullptr);
}

TEST(ParserTest, LetStatementWithType) {
    std::string source = "let x: int = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    LetStmt* letStmt = dynamic_cast<LetStmt*>(statements[0].get());
    ASSERT_NE(letStmt, nullptr);
    EXPECT_EQ(letStmt->name.lexeme, "x");
    ASSERT_TRUE(letStmt->type.has_value());
    EXPECT_EQ(letStmt->type->baseType.lexeme, "int");
}

TEST(ParserTest, LogicalAndExpression) {
    std::string source = "true && false;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
    ASSERT_NE(exprStmt, nullptr);
    ASTPrinter printer;
    std::string result = printer.print(*exprStmt->expression);
    ASSERT_EQ(result, "(&& true false)");
}

TEST(ParserTest, LogicalOrExpression) {
    std::string source = "true || false;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
    ASSERT_NE(exprStmt, nullptr);
    ASTPrinter printer;
    std::string result = printer.print(*exprStmt->expression);
    ASSERT_EQ(result, "(|| true false)");
}

TEST(ParserTest, FunctionDeclaration) {
    std::string source = "func add(a: int, b: int) { return a + b; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* funcStmt = dynamic_cast<FunctionStmt*>(stmts[0].get());
    ASSERT_NE(funcStmt, nullptr);
    EXPECT_EQ(funcStmt->name.lexeme, "add");
    ASSERT_EQ(funcStmt->params.size(), 2);
    EXPECT_EQ(funcStmt->params[0].first.lexeme, "a");
    EXPECT_EQ(funcStmt->params[1].first.lexeme, "b");
}

TEST(ParserTest, ReturnStatement) {
    std::string source = "func add(a: int, b: int) { return a + b; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* funcStmt = dynamic_cast<FunctionStmt*>(stmts[0].get());
    ASSERT_NE(funcStmt, nullptr);
    ASSERT_EQ(funcStmt->body.size(), 1);
    auto* returnStmt = dynamic_cast<ReturnStmt*>(funcStmt->body[0].get());
    ASSERT_NE(returnStmt, nullptr);
}

TEST(ParserTest, CallExpression) {
    std::string source = "add(1, 2);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
    ASSERT_NE(exprStmt, nullptr);
    auto* callExpr = dynamic_cast<CallExpr*>(exprStmt->expression.get());
    ASSERT_NE(callExpr, nullptr);
}

TEST(ParserTest, StructDeclaration) {
    std::string source = "struct Point { let x: int; let y: int; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* structStmt = dynamic_cast<StructStmt*>(stmts[0].get());
    ASSERT_NE(structStmt, nullptr);
    EXPECT_EQ(structStmt->name.lexeme, "Point");
    ASSERT_EQ(structStmt->fields.size(), 2);
    EXPECT_EQ(structStmt->fields[0]->name.lexeme, "x");
    EXPECT_EQ(structStmt->fields[1]->name.lexeme, "y");
}

TEST(ParserTest, LetStatementWithReferenceType) {
    std::string source = "let x: &int = &y;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    LetStmt* letStmt = dynamic_cast<LetStmt*>(statements[0].get());
    ASSERT_NE(letStmt, nullptr);
    EXPECT_EQ(letStmt->name.lexeme, "x");
    ASSERT_TRUE(letStmt->type.has_value());
    EXPECT_EQ(letStmt->type->baseType.lexeme, "int");
    EXPECT_TRUE(letStmt->type->isReference);
    EXPECT_FALSE(letStmt->type->isMutable);
}

TEST(ParserTest, FunctionDeclarationWithReferenceTypes) {
    std::string source = "func foo(a: &int, b: &mut string) -> &bool {}";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* funcStmt = dynamic_cast<FunctionStmt*>(stmts[0].get());
    ASSERT_NE(funcStmt, nullptr);

    ASSERT_EQ(funcStmt->params.size(), 2);
    EXPECT_EQ(funcStmt->params[0].first.lexeme, "a");
    EXPECT_EQ(funcStmt->params[0].second.baseType.lexeme, "int");
    EXPECT_TRUE(funcStmt->params[0].second.isReference);
    EXPECT_FALSE(funcStmt->params[0].second.isMutable);

    EXPECT_EQ(funcStmt->params[1].first.lexeme, "b");
    EXPECT_EQ(funcStmt->params[1].second.baseType.lexeme, "string");
    EXPECT_TRUE(funcStmt->params[1].second.isReference);
    EXPECT_TRUE(funcStmt->params[1].second.isMutable);

    ASSERT_TRUE(funcStmt->returnType.has_value());
    EXPECT_EQ(funcStmt->returnType->baseType.lexeme, "bool");
    EXPECT_TRUE(funcStmt->returnType->isReference);
    EXPECT_FALSE(funcStmt->returnType->isMutable);
}

TEST(ParserTest, GenericFunctionDeclaration) {
    std::string source = "func foo<T, U>(a: T, b: U) {}";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* funcStmt = dynamic_cast<FunctionStmt*>(stmts[0].get());
    ASSERT_NE(funcStmt, nullptr);

    ASSERT_EQ(funcStmt->generics.size(), 2);
    EXPECT_EQ(funcStmt->generics[0].lexeme, "T");
    EXPECT_EQ(funcStmt->generics[1].lexeme, "U");

    ASSERT_EQ(funcStmt->params.size(), 2);
    EXPECT_EQ(funcStmt->params[0].first.lexeme, "a");
    EXPECT_EQ(funcStmt->params[0].second.baseType.lexeme, "T");
    EXPECT_EQ(funcStmt->params[1].first.lexeme, "b");
    EXPECT_EQ(funcStmt->params[1].second.baseType.lexeme, "U");
}
