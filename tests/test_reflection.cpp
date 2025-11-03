#include "gtest/gtest.h"
#include "../src/Transpiler.h"
#include "../src/Lexer.h"
#include "../src/Parser.h"

TEST(ReflectionTest, GetFields) {
    std::string source =
        "import reflect;\n"
        "struct Point { let x: int = 1; let y: int = 2; }\n"
        "let p = Point {};\n"
        "let fields = reflect::field::get_fields(p);\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n"
        "#include <any>\n"
        "struct chtholly_field {\n"
        "    std::string name;\n"
        "    std::string type;\n"
        "    std::any value;\n"
        "};\n\n"
        "struct Point {\n"
        "    const int x = 1;\n"
        "    const int y = 2;\n"
        "};\n\n"
        "int main() {\n"
        "    const Point p = {};\n"
        "    const auto fields = std::vector<chtholly_field>{\n"
        "        chtholly_field{\"x\", \"int\", p.x},\n"
        "        chtholly_field{\"y\", \"int\", p.y},\n"
        "    };\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}
