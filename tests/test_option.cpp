#include "gtest/gtest.h"
#include "../src/Lexer.h"
#include "../src/PrattParser.h"
#include "../src/AST/ASTPrinter.h"
#include "../src/Resolver.h"
#include "../src/Transpiler.h"
#include "../src/Error.h"

class OptionTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(OptionTest, OptionDeclaration) {
    std::string source = "let a: option<int> = none;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    auto* letStmt = dynamic_cast<LetStmt*>(stmts[0].get());
    ASSERT_NE(letStmt, nullptr);
    ASSERT_TRUE(letStmt->type.has_value());
    ASSERT_EQ(letStmt->type->baseType.lexeme, "option");
    ASSERT_EQ(letStmt->type->params.size(), 1);
    ASSERT_EQ(letStmt->type->params[0].baseType.lexeme, "int");
}

TEST_F(OptionTest, OptionResolution) {
    std::string source = "let a: option<int> = none;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(OptionTest, OptionTranspilation) {
    std::string source = "let a: option<int> = none;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n"
        "#include <optional>\n\n"
        "int main() {\n"
        "    const std::optional<int> a = std::nullopt;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST_F(OptionTest, UnwarpCall) {
    std::string source =
        "let a: option<int> = 10;\n"
        "let b: int = a.unwarp();\n"
        "print(b);";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n"
        "#include <optional>\n\n"
        "int main() {\n"
        "    const std::optional<int> a = 10;\n"
        "    const int b = a.value();\n"
        "    std::cout << b << std::endl;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}

TEST_F(OptionTest, UnwarpOrCall) {
    std::string source =
        "let a: option<int> = none;\n"
        "let b: int = a.unwarp_or(5);\n"
        "print(b);";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n"
        "#include <optional>\n\n"
        "int main() {\n"
        "    const std::optional<int> a = std::nullopt;\n"
        "    const int b = a.value_or(5);\n"
        "    std::cout << b << std::endl;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}
