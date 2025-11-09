#include <gtest/gtest.h>
#include "TestHelpers.h"

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
        func main() -> result<int, string> {
            return result::pass(0);
        }
    )";
    std::string expected = "return result<int, std::string>::pass(0);";
    std::string transpiled = compile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
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
    std::string expected_y = "const bool y = x.is_pass();";
    std::string expected_z = "const bool z = x.is_fail();";
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
    std::string expected_a = "const bool a = chtholly_is_pass(x);";
    std::string expected_b = "const bool b = chtholly_is_fail(x);";
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
    std::string expected_a = "const bool a = chtholly_is_fail(x);";
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
