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

TEST(CodeGen, TestLetStatements) {
    std::string input = R"(
        let x = 5;
        mut y = 10;
        let z: int = 15;
        mut w: string = "hello";
    )";
    std::string expected = R"(const auto x = 5;
auto y = 10;
const int z = 15;
string w = "hello";
)";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}
