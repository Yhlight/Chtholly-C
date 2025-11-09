#include <gtest/gtest.h>
#include "TestHelpers.h"

std::string transpile(const std::string& source);

TEST(TestResult, SimpleResult) {
    std::string source = R"(
        let x: result<int, string> = result::pass(10);
    )";
    std::string expected = "const result<int, std::string> x = result<int, std::string>::pass(10);";
    std::string transpiled = transpile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(TestResult, TypeInferenceFail) {
    std::string source = R"(
        let x = result::fail("error");
    )";
    std::string expected = "const result<auto, std::string> x = result<auto, std::string>::fail(\"error\");";
    std::string transpiled = transpile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST(TestResult, SimpleResultFail) {
    std::string source = R"(
        let x: result<int, string> = result::fail("error");
    )";
    std::string expected = "const result<int, std::string> x = result<int, std::string>::fail(\"error\");";
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
    std::string expected = "const result<int, std::string> x = result<int, std::string>::pass(10);";
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

TEST(TestResult, StaticIsPassAndIsFail) {
    std::string source = R"(
        let x: result<int, string> = result::pass(10);
        let a = result::is_pass(x);
        let b = result::is_fail(x);
    )";
    std::string expected_x = "const result<int, std::string> x = result<int, std::string>::pass(10);";
    std::string expected_a = "const bool a = (x).is_pass();";
    std::string expected_b = "const bool b = (x).is_fail();";
    std::string transpiled = transpile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected_x.erase(std::remove_if(expected_x.begin(), expected_x.end(), ::isspace), expected_x.end());
    expected_a.erase(std::remove_if(expected_a.begin(), expected_a.end(), ::isspace), expected_a.end());
    expected_b.erase(std::remove_if(expected_b.begin(), expected_b.end(), ::isspace), expected_b.end());
    ASSERT_NE(transpiled.find(expected_x), std::string::npos);
    ASSERT_NE(transpiled.find(expected_a), std::string::npos);
    ASSERT_NE(transpiled.find(expected_b), std::string::npos);
}

TEST(TestResult, TypeInference) {
    std::string source = R"(
        let x = result::pass(10);
    )";
    std::string expected = "const result<int, auto> x = result<int, auto>::pass(10);";
    std::string transpiled = transpile(source);
    std::cout << "Transpiled output:\n" << transpiled << std::endl;
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}
