#include <gtest/gtest.h>
#include "Parser.h"
#include "CodeGen.h"

void checkParserErrors(Chtholly::Parser& p) {
    const auto& errors = p.errors();
    if (errors.empty()) {
        return;
    }

    std::cerr << "Parser has " << errors.size() << " errors" << std::endl;
    for (const auto& msg : errors) {
        std::cerr << "parser error: " << msg << std::endl;
    }
    FAIL();
}

TEST(CodeGen, TestLetStatementWithType) {
    std::string input = "let x: int = 5;";
    std::string expected = "int x = 5;\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}

TEST(CodeGen, TestFunctionLiteralWithType) {
    std::string input = "func(x: int, y: string) -> bool { return x > y; }";
    std::string expected = "[&](int x, string y) -> bool {\nreturn (x > y);\n}\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}
