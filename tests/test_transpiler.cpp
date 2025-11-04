#include <gtest/gtest.h>
#include "../src/Transpiler.h"
#include "../src/PrattParser.h"
#include "../src/Lexer.h"

TEST(TranspilerTest, SimplePrint) {
    std::string source = "print(1 + 2);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
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
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "int main() {\n"
        "    const auto x = 10;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, Function) {
    std::string source = "func add(a: &int, b: &int) -> int { return a + b; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "int add(const int& a, const int& b) {\n"
        "    return a + b;\n"
        "}\n\n"
        "int main() {\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, Struct) {
    std::string source = "struct Point { mut x: int; mut y: int; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "struct Point {\n"
        "    int x;\n"
        "    int y;\n"
        "};\n\n"
        "int main() {\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}
