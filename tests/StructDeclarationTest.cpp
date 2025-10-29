#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"

TEST(StructDeclarationTest, CorrectlyParsesStructDeclaration) {
    std::string source =
        "struct Test {"
        "  name: string,"
        "  id: int,"
        "  add(x: int, y: int) -> int { }"
        "}";

    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.tokenize();
    Chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<Chtholly::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    Chtholly::ASTPrinter printer;
    std::string result = printer.print(statements);

    std::string expected =
        "(struct Test { (var name: string;)\n (var id: int;)\n (func add(x: int, y: int) -> int { \n}) })";
    ASSERT_EQ(result, expected);
}

TEST(StructDeclarationTest, CorrectlyParsesStructDeclarationWithoutTrailingComma) {
    std::string source =
        "struct Test {"
        "  name: string,"
        "  id: int"
        "}";

    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.tokenize();
    Chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<Chtholly::Stmt>> statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);

    Chtholly::ASTPrinter printer;
    std::string result = printer.print(statements);

    std::string expected =
        "(struct Test { (var name: string;)\n (var id: int;)\n })";
    ASSERT_EQ(result, expected);
}
