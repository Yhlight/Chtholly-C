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

TEST(CodeGen, TestIfExpression) {
    std::string input = "if (x < y) { return x; }";
    std::string expected = "if ((x < y)) {\nreturn x;\n}\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}

TEST(CodeGen, TestIfElseExpression) {
    std::string input = "if (x < y) { return x; } else { return y; }";
    std::string expected = "if ((x < y)) {\nreturn x;\n}\nelse {\nreturn y;\n}\n";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}
