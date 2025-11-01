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
        result += printer.print(*stmt);
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
