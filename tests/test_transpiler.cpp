#include <gtest/gtest.h>
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include "../src/Transpiler.h"

TEST(TranspilerTest, LetStatement) {
    std::string source = "let x = 10;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Transpiler transpiler;
    std::string result = transpiler.transpile(statements);

    EXPECT_EQ(result, "auto x = 10;\n");
}

TEST(TranspilerTest, LetStatementNoInitializer) {
    std::string source = "let y;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Transpiler transpiler;
    std::string result = transpiler.transpile(statements);

    EXPECT_EQ(result, "auto y;\n");
}

TEST(TranspilerTest, Expression) {
    std::string source = "let x = (10 + 20) * -30;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Transpiler transpiler;
    std::string result = transpiler.transpile(statements);

    EXPECT_EQ(result, "auto x = ((10 + 20) * (-30));\n");
}
