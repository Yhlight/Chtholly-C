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
        "class chtholly_field {\n"
        "public:\n"
        "    chtholly_field(std::string name, std::string type, std::any value)\n"
        "        : name(std::move(name)), type(std::move(type)), value(std::move(value)) {}\n\n"
        "    std::string get_name() const { return name; }\n"
        "    std::string get_type() const { return type; }\n"
        "    std::any get_value() const { return value; }\n\n"
        "private:\n"
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
        "        chtholly_field(\"x\", \"int\", p.x),\n"
        "        chtholly_field(\"y\", \"int\", p.y),\n"
        "    };\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(ReflectionTest, GetField) {
    std::string source =
        "import reflect;\n"
        "struct Point { let x: int = 1; }\n"
        "let p = Point {};\n"
        "let field = reflect::field::get_field(p, \"x\");\n";
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
        "class chtholly_field {\n"
        "public:\n"
        "    chtholly_field(std::string name, std::string type, std::any value)\n"
        "        : name(std::move(name)), type(std::move(type)), value(std::move(value)) {}\n\n"
        "    std::string get_name() const { return name; }\n"
        "    std::string get_type() const { return type; }\n"
        "    std::any get_value() const { return value; }\n\n"
        "private:\n"
        "    std::string name;\n"
        "    std::string type;\n"
        "    std::any value;\n"
        "};\n\n"
        "struct Point {\n"
        "    const int x = 1;\n"
        "};\n\n"
        "int main() {\n"
        "    const Point p = {};\n"
        "    const auto field = chtholly_field(\"x\", \"int\", p.x);\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}
