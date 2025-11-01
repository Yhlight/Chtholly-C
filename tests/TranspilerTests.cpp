#include <gtest/gtest.h>
#include "Transpiler.h"
#include "Parser.h"
#include "Lexer.h"
#include <vector>
#include <memory>

// Helper to remove whitespace and newlines for easier comparison
static std::string_view trim(std::string_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(" \t\n\r\f\v"), s.size()));
    s.remove_suffix(std::min(s.size() - s.find_last_not_of(" \t\n\r\f\v") - 1, s.size()));
    return s;
}

static std::string parseAndTranspileStmts(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    Transpiler transpiler;
    return transpiler.transpile(stmts);
}

TEST(TranspilerStatementTest, TranspilesLetStatement) {
    std::string result = parseAndTranspileStmts("let a = 10;");
    std::string expected = R"(
#include <iostream>
#include <string>
#include <variant>

int main() {
    auto a = 10.000000;
    return 0;
}
)";
    ASSERT_EQ(trim(result), trim(expected));
}

TEST(TranspilerStatementTest, TranspilesPrintStatement) {
    std::string result = parseAndTranspileStmts("print \"hello\";");
    std::string expected = R"(
#include <iostream>
#include <string>
#include <variant>

int main() {
    std::cout << (std::string("hello")) << std::endl;
    return 0;
}
)";
    ASSERT_EQ(trim(result), trim(expected));
}
