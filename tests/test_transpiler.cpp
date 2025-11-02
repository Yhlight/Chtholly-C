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
        "    auto x = 10;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}
