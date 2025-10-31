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

TEST(OperatorTest, Addition) {
    std::string source = "1 + 2;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string code = transpiler.transpile(stmts);

    std::string expected_output = "#include <iostream>\n"
                                  "#include <string>\n\n"
                                  "int main(int argc, char* argv[]) {\n"
                                  "    (1 + 2);\n"
                                  "    return 0;\n"
                                  "}\n";
    EXPECT_EQ(code, expected_output);
}

TEST(OperatorTest, Subtraction) {
    std::string source = "3 - 1;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string code = transpiler.transpile(stmts);

    std::string expected_output = "#include <iostream>\n"
                                  "#include <string>\n\n"
                                  "int main(int argc, char* argv[]) {\n"
                                  "    (3 - 1);\n"
                                  "    return 0;\n"
                                  "}\n";
    EXPECT_EQ(code, expected_output);
}

TEST(OperatorTest, Multiplication) {
    std::string source = "2 * 3;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string code = transpiler.transpile(stmts);

    std::string expected_output = "#include <iostream>\n"
                                  "#include <string>\n\n"
                                  "int main(int argc, char* argv[]) {\n"
                                  "    (2 * 3);\n"
                                  "    return 0;\n"
                                  "}\n";
    EXPECT_EQ(code, expected_output);
}

TEST(OperatorTest, Division) {
    std::string source = "4 / 2;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    std::string code = transpiler.transpile(stmts);

    std::string expected_output = "#include <iostream>\n"
                                  "#include <string>\n\n"
                                  "int main(int argc, char* argv[]) {\n"
                                  "    (4 / 2);\n"
                                  "    return 0;\n"
                                  "}\n";
    EXPECT_EQ(code, expected_output);
}

TEST(OperatorTest, Precedence) {
    std::string source = "1 + 2 * 3;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(stmts);

    EXPECT_EQ(result, "(; (+ 1 (* 2 3)))");
}
