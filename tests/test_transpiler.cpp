#include <gtest/gtest.h>
#include "../src/Transpiler.h"
#include "../src/Lexer.h"
#include "../src/Parser.h"
#include "../src/Resolver.h"

TEST(TranspilerTest, SimplePrint) {
    std::string source = "print(1 + 2);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "int main() {\n"
        "    std::cout << (1 + 2) << std::endl;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, StructWithAccessModifiers) {
    std::string source = "struct Point { public let x: int; private let y: int; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "struct Point {\n"
        "public:\n"
        "    int x;\n"
        "private:\n"
        "    int y;\n"
        "};\n\n";
    ASSERT_TRUE(result.find(expected) != std::string::npos);
}

TEST(TranspilerTest, Lambda) {
    std::string source = "let x: function(int) -> int = [](a: int) -> int { return a; }; print(x(1));";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n"
        "#include <functional>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "int main() {\n"
        "    std::function<int(int)> x = [](int a) -> int {\n"
        "        return a;\n"
        "    };\n"
        "    std::cout << x(1) << std::endl;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, GenericFunction) {
    std::string source = "func foo<T>(a: T) -> T { return a; } print(foo<int>(1));";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "template<typename T>\n"
        "T foo(T a) {\n"
        "    return a;\n"
        "}\n\n"
        "int main() {\n"
        "    std::cout << foo<int>(1) << std::endl;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, LetStatementWithReference) {
    std::string source = "let x: &int = &y;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "int main() {\n"
        "    const int& x = &y;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, Struct) {
    std::string source = "struct Point { mut x: int; mut y: int; } let p: Point; p.x = 1; print(p.x);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "struct Point {\n"
        "public:\n"
        "    int x;\n"
        "    int y;\n"
        "};\n\n"
        "int main() {\n"
        "    Point p;\n"
        "    p.x = 1;\n"
        "    std::cout << p.x << std::endl;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, Function) {
    std::string source = "func add(a: &int, b: &int) -> int { return a + b; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "int add(const int& a, const int& b) {\n"
        "    return a + b;\n"
        "}\n\n"
        "int main() {\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, LetStatementWithType) {
    std::string source = "let x: int = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "int main() {\n"
        "    const int x = 10;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, StringDeclaration) {
    std::string source = "let s: string = \"hello\";";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "int main() {\n"
        "    const std::string s = \"hello\";\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, LogicalAnd) {
    std::string source = "print(true && false);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "int main() {\n"
        "    std::cout << (true && false) << std::endl;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, LogicalOr) {
    std::string source = "print(true || false);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "int main() {\n"
        "    std::cout << (true || false) << std::endl;\n"
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
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "int main() {\n"
        "    const double x = 10;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST(TranspilerTest, Input) {
    std::string source = "let name = input();";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    Transpiler transpiler(resolver);
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "std::string chtholly_input() {\n"
        "    std::string line;\n"
        "    std::getline(std::cin, line);\n"
        "    return line;\n"
        "}\n\n"
        "int main() {\n"
        "    const auto name = chtholly_input();\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}
