#include "gtest/gtest.h"
#include "../src/Transpiler.h"
#include "../src/Lexer.h"
#include "../src/Parser.h"

TEST(MetaTest, IsInt) {
    std::string source =
        "import meta;\n"
        "let x = 10;\n"
        "let is_int = meta::is_int(x);\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "int main() {\n"
        "    const auto x = 10;\n"
        "    const auto is_int = true;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(MetaTest, IsString) {
    std::string source =
        "import meta;\n"
        "let s = \"hello\";\n"
        "let is_string = meta::is_string(s);\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "int main() {\n"
        "    const auto s = \"hello\";\n"
        "    const auto is_string = true;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(MetaTest, IsStruct) {
    std::string source =
        "import meta;\n"
        "struct Point { let x: int; }\n"
        "let p = Point {};\n"
        "let is_struct = meta::is_struct(p);\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "struct Point {\n"
        "    const int x;\n"
        "};\n\n"
        "int main() {\n"
        "    const Point p = {};\n"
        "    const auto is_struct = true;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}
