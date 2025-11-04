#include "gtest/gtest.h"
#include "../src/Lexer.h"
#include "../src/PrattParser.h"
#include "../src/AST/ASTPrinter.h"
#include "../src/Resolver.h"
#include "../src/Transpiler.h"
#include "../src/Error.h"

class EnumTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(EnumTest, EnumDeclaration) {
    std::string source = "enum Color { RED, GREEN, BLUE }";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);
    ASSERT_NE(dynamic_cast<EnumStmt*>(stmts[0].get()), nullptr);
}

TEST_F(EnumTest, EnumResolution) {
    std::string source = "enum Color { RED, GREEN, BLUE } let a = Color::RED;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Resolver resolver;
    resolver.resolve(stmts);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(EnumTest, EnumTranspilation) {
    std::string source = "enum Color { RED, GREEN, BLUE } let a = Color::RED;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    PrattParser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string result = transpiler.transpile(stmts);
    std::string expected =
        "#include <iostream>\n"
        "#include <variant>\n"
        "#include <string>\n\n"
        "enum class Color {\n"
        "    RED,\n"
        "    GREEN,\n"
        "    BLUE\n"
        "};\n\n"
        "int main() {\n"
        "    const auto a = Color::RED;\n"
        "    return 0;\n"
        "}\n";
    ASSERT_EQ(result, expected);
}
