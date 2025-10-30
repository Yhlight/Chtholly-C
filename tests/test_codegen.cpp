#include <gtest/gtest.h>
#include "Parser.h"
#include "CodeGen.h"

TEST(CodeGen, TestExpressions) {
    std::vector<std::pair<std::string, std::string>> tests = {
        {"5;", "5;\n"},
        {"true;", "true;\n"},
        {"false;", "false;\n"},
        {"!true;", "(!true);\n"},
        {"-5;", "(-5);\n"},
        {"5 + 5;", "(5 + 5);\n"},
        {"5 - 5;", "(5 - 5);\n"},
        {"5 * 5;", "(5 * 5);\n"},
        {"5 / 5;", "(5 / 5);\n"},
        {"5 > 5;", "(5 > 5);\n"},
        {"5 < 5;", "(5 < 5);\n"},
        {"5 == 5;", "(5 == 5);\n"},
        {"5 != 5;", "(5 != 5);\n"},
        {"true == true;", "(true == true);\n"},
        {"true != false;", "(true != false);\n"},
        {"(5 + 5) * 2;", "((5 + 5) * 2);\n"},
    };

    for (const auto& test : tests) {
        Chtholly::Lexer l(test.first);
        Chtholly::Parser p(l);
        auto program = p.parseProgram();
        Chtholly::CodeGen c;
        std::string result = c.generate(program.get());

        ASSERT_EQ(result, test.second);
    }
}
