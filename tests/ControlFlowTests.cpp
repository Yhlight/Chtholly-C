#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"
#include "Transpiler.h"
#include <string>
#include <fstream>
#include <stdexcept>
#include <array>
#include <memory>

TEST(ControlFlowTest, IfStatement) {
    std::string source = "if (true) 1;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASTPrinter printer;
    std::string result = printer.print(stmts);
    EXPECT_EQ(result, "(if true (; 1))");
}

TEST(ControlFlowTest, IfElseStatement) {
    std::string source = "if (true) 1; else 2;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    ASTPrinter printer;
    std::string result = printer.print(stmts);
    EXPECT_EQ(result, "(if true (; 1) else (; 2))");
}

TEST(ControlFlowTest, IfStatementTranspilation) {
    std::string source = "if (true) 1;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string code = transpiler.transpile(stmts);

    std::string expected_output = "#include <iostream>\n"
                                  "#include <string>\n\n"
                                  "int main(int argc, char* argv[]) {\n"
                                  "    if (true)     1;\n"
                                  "    return 0;\n"
                                  "}\n";
    EXPECT_EQ(code, expected_output);
}

TEST(ControlFlowTest, IfElseStatementTranspilation) {
    std::string source = "if (true) 1; else 2;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string code = transpiler.transpile(stmts);

    std::string expected_output = "#include <iostream>\n"
                                  "#include <string>\n\n"
                                  "int main(int argc, char* argv[]) {\n"
                                  "    if (true)     1;\n"
                                  "else     2;\n"
                                  "    return 0;\n"
                                  "}\n";
    EXPECT_EQ(code, expected_output);
}
