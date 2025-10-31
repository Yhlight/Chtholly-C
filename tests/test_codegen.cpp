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

TEST(CodeGen, TestStructStatement) {
    std::string input = R"(
        struct MyStruct {
            public name: string,
            private id: int,

            public add(x: int, y: int) -> int {
                return x + y;
            }
        }
    )";
    std::string expected = R"(struct MyStruct {
public: string name;
private: int id;
public: auto add = [&](int x, int y) -> int {
return (x + y);
}
;
};
)";

    Chtholly::Lexer l(input);
    Chtholly::Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    Chtholly::CodeGen c;
    std::string result = c.generate(program.get());

    ASSERT_EQ(result, expected);
}
