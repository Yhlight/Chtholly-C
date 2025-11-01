#include <gtest/gtest.h>
#include "../src/Parser.h"
#include "../src/AstPrinter.h"
#include "../src/Lexer.h"

using namespace chtholly;

std::string parseAndPrint(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    AstPrinter printer;
    std::string result = "";
    for (const auto& stmt : statements) {
        if (stmt) {
            result += printer.print(*stmt);
        }
    }
    return result;
}


TEST(ParserTest, VarDeclaration) {
    std::string source = "let x = 5;";
    std::string expected = "(var x = 5.000000)";
    ASSERT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, MutVarDeclaration) {
    std::string source = "mut y = \"hello\";";
    std::string expected = "(var y = hello)";
    ASSERT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, VarDeclarationNoInitializer) {
    std::string source = "let z;";
    std::string expected = "(var z)";
    ASSERT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, UnaryExpression) {
    std::string source = "let x = !false;";
    std::string expected = "(var x = (! false))";
    ASSERT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, BinaryExpression) {
    std::string source = "let x = 5 + 3;";
    std::string expected = "(var x = (+ 5.000000 3.000000))";
    ASSERT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, Precedence) {
    std::string source = "let x = 5 + 3 * 2;";
    std::string expected = "(var x = (+ 5.000000 (* 3.000000 2.000000)))";
    ASSERT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, Grouping) {
    std::string source = "let x = (5 + 3) * 2;";
    std::string expected = "(var x = (* (group (+ 5.000000 3.000000)) 2.000000))";
    ASSERT_EQ(parseAndPrint(source), expected);
}
