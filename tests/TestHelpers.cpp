#include "TestHelpers.h"
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"
#include <algorithm>
#include <cctype>

std::string compile(const std::string& source) {
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    chtholly::Transpiler transpiler;
    return transpiler.transpile(statements);
}

#include <gtest/gtest.h>

std::string normalize(const std::string& s) {
    std::string result = s;
    result.erase(std::remove_if(result.begin(), result.end(),
                                [](unsigned char c) { return std::isspace(c); }),
                 result.end());
    return result;
}

void compile_and_expect_error(const std::string& source, const std::string& expected_error_message) {
    try {
        compile(source);
        FAIL() << "Expected std::runtime_error, but no exception was thrown.";
    } catch (const std::runtime_error& e) {
        std::string error_message = e.what();
        ASSERT_TRUE(error_message.find(expected_error_message) != std::string::npos)
            << "Expected error message to contain: " << expected_error_message
            << "\nBut got: " << error_message;
    }
}
