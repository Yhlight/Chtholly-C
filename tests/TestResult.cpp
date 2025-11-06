#include <gtest/gtest.h>
#include "TestHelpers.h"

std::string transpile(const std::string& source);

TEST(TestResult, SimpleResult) {
    std::string source = R"(
        let x: result<int, string> = result::pass(10);
    )";
    std::string expected = "const Result<int, std::string> x = Result<int, std::string>::pass(10);";
    std::string transpiled = transpile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(TestResult, IsPassAndIsFail) {
    std::string source = R"(
        let x: result<int, string> = result::pass(10);
        let y = x.is_pass();
        let z = x.is_fail();
    )";
    std::string expected = "const Result<int, std::string> x = Result<int, std::string>::pass(10);";
    std::string expected_y = "const bool y = x.is_pass();";
    std::string expected_z = "const bool z = x.is_fail();";
    std::string transpiled = transpile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    expected_y.erase(std::remove_if(expected_y.begin(), expected_y.end(), ::isspace), expected_y.end());
    expected_z.erase(std::remove_if(expected_z.begin(), expected_z.end(), ::isspace), expected_z.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
    ASSERT_NE(transpiled.find(expected_y), std::string::npos);
    ASSERT_NE(transpiled.find(expected_z), std::string::npos);
}

TEST(TestResult, TypeInference) {
    std::string source = R"(
        let x = result::pass(10);
    )";
    std::string expected = "const Result<int, auto> x = Result<int, auto>::pass(10);";
    std::string transpiled = transpile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}
