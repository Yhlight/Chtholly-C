#include <gtest/gtest.h>
#include "../src/Transpiler.h"
#include "../src/Lexer.h"
#include "../src/Parser.h"

TEST(TranspilerTest, SimplePrint) {
    std::string source = "print 1 + 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n\n"
        "int main() {\n"
        "    std::cout << (1 + 2) << std::endl;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, LetStatement) {
    std::string source = "let x = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n\n"
        "int main() {\n"
        "    const auto x = 10;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, MutLetStatement) {
    std::string source = "mut x = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n\n"
        "int main() {\n"
        "    auto x = 10;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, Assignment) {
    std::string source = "mut x = 10; x = 20;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n\n"
        "int main() {\n"
        "    auto x = 10;\n"
        "    x = 20;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, Block) {
    std::string source = "{ let x = 1; print x; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n\n"
        "int main() {\n"
        "    {\n"
        "    const auto x = 1;\n"
        "    std::cout << x << std::endl;\n"
        "    }\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}
