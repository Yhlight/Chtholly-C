#include <gtest/gtest.h>
#include "TestHelpers.h"
#include "Lexer.h"
#include <iostream>

// Helper function to convert TokenType to string for debugging
std::string tokenTypeToString(chtholly::TokenType type) {
    switch (type) {
        case chtholly::TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case chtholly::TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case chtholly::TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case chtholly::TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case chtholly::TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case chtholly::TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case chtholly::TokenType::COMMA: return "COMMA";
        case chtholly::TokenType::DOT: return "DOT";
        case chtholly::TokenType::MINUS: return "MINUS";
        case chtholly::TokenType::PLUS: return "PLUS";
        case chtholly::TokenType::SLASH: return "SLASH";
        case chtholly::TokenType::STAR: return "STAR";
        case chtholly::TokenType::PERCENT: return "PERCENT";
        case chtholly::TokenType::SEMICOLON: return "SEMICOLON";
        case chtholly::TokenType::COLON: return "COLON";
        case chtholly::TokenType::QUESTION: return "QUESTION";
        case chtholly::TokenType::STAR_STAR: return "STAR_STAR";
        case chtholly::TokenType::BANG: return "BANG";
        case chtholly::TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case chtholly::TokenType::EQUAL: return "EQUAL";
        case chtholly::TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case chtholly::TokenType::GREATER: return "GREATER";
        case chtholly::TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case chtholly::TokenType::LESS: return "LESS";
        case chtholly::TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case chtholly::TokenType::AMPERSAND: return "AMPERSAND";
        case chtholly::TokenType::AND: return "AND";
        case chtholly::TokenType::PIPE: return "PIPE";
        case chtholly::TokenType::OR: return "OR";
        case chtholly::TokenType::PLUS_EQUAL: return "PLUS_EQUAL";
        case chtholly::TokenType::MINUS_EQUAL: return "MINUS_EQUAL";
        case chtholly::TokenType::STAR_EQUAL: return "STAR_EQUAL";
        case chtholly::TokenType::SLASH_EQUAL: return "SLASH_EQUAL";
        case chtholly::TokenType::PERCENT_EQUAL: return "PERCENT_EQUAL";
        case chtholly::TokenType::ARROW: return "ARROW";
        case chtholly::TokenType::COLON_COLON: return "COLON_COLON";
        case chtholly::TokenType::PLUS_PLUS: return "PLUS_PLUS";
        case chtholly::TokenType::MINUS_MINUS: return "MINUS_MINUS";
        case chtholly::TokenType::IDENTIFIER: return "IDENTIFIER";
        case chtholly::TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case chtholly::TokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case chtholly::TokenType::CHAR_LITERAL: return "CHAR_LITERAL";
        case chtholly::TokenType::IF: return "IF";
        case chtholly::TokenType::ELSE: return "ELSE";
        case chtholly::TokenType::FOR: return "FOR";
        case chtholly::TokenType::WHILE: return "WHILE";
        case chtholly::TokenType::SWITCH: return "SWITCH";
        case chtholly::TokenType::CASE: return "CASE";
        case chtholly::TokenType::DEFAULT: return "DEFAULT";
        case chtholly::TokenType::BREAK: return "BREAK";
        case chtholly::TokenType::FALLTHROUGH: return "FALLTHROUGH";
        case chtholly::TokenType::FUNC: return "FUNC";
        case chtholly::TokenType::RETURN: return "RETURN";
        case chtholly::TokenType::STRUCT: return "STRUCT";
        case chtholly::TokenType::ENUM: return "ENUM";
        case chtholly::TokenType::IMPL: return "IMPL";
        case chtholly::TokenType::TRAIT: return "TRAIT";
        case chtholly::TokenType::LET: return "LET";
        case chtholly::TokenType::MUT: return "MUT";
        case chtholly::TokenType::PUBLIC: return "PUBLIC";
        case chtholly::TokenType::PRIVATE: return "PRIVATE";
        case chtholly::TokenType::SELF: return "SELF";
        case chtholly::TokenType::NONE: return "NONE";
        case chtholly::TokenType::TRUE: return "TRUE";
        case chtholly::TokenType::FALSE: return "FALSE";
        case chtholly::TokenType::IMPORT: return "IMPORT";
        case chtholly::TokenType::PRINT: return "PRINT";
        case chtholly::TokenType::INPUT: return "INPUT";
        case chtholly::TokenType::TYPE_CAST: return "TYPE_CAST";
        case chtholly::TokenType::IOSTREAM: return "IOSTREAM";
        case chtholly::TokenType::FILESYSTEM: return "FILESYSTEM";
        case chtholly::TokenType::FS_READ: return "FS_READ";
        case chtholly::TokenType::FS_WRITE: return "FS_WRITE";
        case chtholly::TokenType::OPERATOR: return "OPERATOR";
        case chtholly::TokenType::REFLECT: return "REFLECT";
        case chtholly::TokenType::UTIL: return "UTIL";
        case chtholly::TokenType::META: return "META";
        case chtholly::TokenType::INT: return "INT";
        case chtholly::TokenType::INT8: return "INT8";
        case chtholly::TokenType::INT16: return "INT16";
        case chtholly::TokenType::INT32: return "INT32";
        case chtholly::TokenType::INT64: return "INT64";
        case chtholly::TokenType::UINT: return "UINT";
        case chtholly::TokenType::UINT8: return "UINT8";
        case chtholly::TokenType::UINT16: return "UINT16";
        case chtholly::TokenType::UINT32: return "UINT32";
        case chtholly::TokenType::UINT64: return "UINT64";
        case chtholly::TokenType::CHAR_TYPE: return "CHAR_TYPE";
        case chtholly::TokenType::DOUBLE: return "DOUBLE";
        case chtholly::TokenType::FLOAT: return "FLOAT";
        case chtholly::TokenType::LONG_DOUBLE: return "LONG_DOUBLE";
        case chtholly::TokenType::VOID: return "VOID";
        case chtholly::TokenType::BOOL: return "BOOL";
        case chtholly::TokenType::STRING_TYPE: return "STRING_TYPE";
        case chtholly::TokenType::ARRAY: return "ARRAY";
        case chtholly::TokenType::FUNCTION: return "FUNCTION";
        case chtholly::TokenType::OPTION: return "OPTION";
        case chtholly::TokenType::RESULT: return "RESULT";
        case chtholly::TokenType::ERROR: return "ERROR";
        case chtholly::TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "UNKNOWN";
    }
}

class TestResult : public ::testing::Test {};

TEST_F(TestResult, SimpleResult) {
    std::string source = R"(
        let x: result<int, string> = result::pass(10);
    )";
    std::string expected = "const result<int, std::string> x = result<int, std::string>::pass(10);";
    std::string transpiled = compile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestResult, ReturnResultPass) {
    std::string source = R"(
        func my_func() -> result<int, string> {
            return result::pass(10);
        }
    )";
    std::string expected = "return result<int, std::string>::pass(10);";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestResult, TypeInferenceFail) {
    std::string source = R"(
        let x = result::fail("error");
    )";
    std::string expected = "const result<auto, std::string> x = result<auto, std::string>::fail(\"error\");";
    std::string transpiled = compile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestResult, SimpleResultFail) {
    std::string source = R"(
        let x: result<int, string> = result::fail("error");
    )";
    std::string expected = "const result<int, std::string> x = result<int, std::string>::fail(\"error\");";
    std::string transpiled = compile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestResult, IsPassAndIsFail) {
    std::string source = R"(
        let x: result<int, string> = result::pass(10);
        let y = x.is_pass();
        let z = x.is_fail();
    )";
    std::string expected = "const result<int, std::string> x = result<int, std::string>::pass(10);";
    std::string expected_y = "const auto y = x.is_pass();";
    std::string expected_z = "const auto z = x.is_fail();";
    std::string transpiled = compile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    expected_y.erase(std::remove_if(expected_y.begin(), expected_y.end(), ::isspace), expected_y.end());
    expected_z.erase(std::remove_if(expected_z.begin(), expected_z.end(), ::isspace), expected_z.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
    ASSERT_NE(transpiled.find(expected_y), std::string::npos);
    ASSERT_NE(transpiled.find(expected_z), std::string::npos);
}
TEST_F(TestResult, StaticIsPassAndIsFail) {
    std::string source = R"(
        let x: result<int, string> = result::pass(10);
        let a = result::is_pass(x);
        let b = result::is_fail(x);
    )";
    std::string expected_a = "const auto a = chtholly_is_pass(x);";
    std::string expected_b = "const auto b = chtholly_is_fail(x);";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected_a.erase(std::remove_if(expected_a.begin(), expected_a.end(), ::isspace), expected_a.end());
    expected_b.erase(std::remove_if(expected_b.begin(), expected_b.end(), ::isspace), expected_b.end());
    ASSERT_NE(transpiled.find(expected_a), std::string::npos);
    ASSERT_NE(transpiled.find(expected_b), std::string::npos);
}
TEST_F(TestResult, StaticIsFailWithNonResult) {
    std::string source = R"(
        let x = 10;
        let a = result::is_fail(x);
    )";
    std::string expected_a = "const auto a = chtholly_is_fail(x);";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected_a.erase(std::remove_if(expected_a.begin(), expected_a.end(), ::isspace), expected_a.end());
    ASSERT_NE(transpiled.find(expected_a), std::string::npos);
}

TEST_F(TestResult, TypeInference) {
    std::string source = R"(
        let x = result::pass(10);
    )";
    std::string expected = "const result<int, auto> x = result<int, auto>::pass(10);";
    std::string transpiled = compile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}
