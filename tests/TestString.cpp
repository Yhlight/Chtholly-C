#include <gtest/gtest.h>
#include "TestHelpers.h"

class TestString : public ::testing::Test {};

TEST_F(TestString, LenFunction) {
    std::string source = R"(
        import string;
        let s = "hello";
        let len = string::len(s);
    )";
    std::string expected = "const int len = s.length();";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestString, FindFunctionFound) {
    std::string source = R"(
        import string;
        let s = "hello world";
        let pos = string::find(s, "world");
    )";
    std::string expected = "const std::optional<int> pos = (s.find(\"world\") == std::string::npos) ? std::nullopt : std::optional<int>(s.find(\"world\"));";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestString, FindFunctionNotFound) {
    std::string source = R"(
        import string;
        let s = "hello world";
        let pos = string::find(s, "galaxy");
    )";
    std::string expected = "const std::optional<int> pos = (s.find(\"galaxy\") == std::string::npos) ? std::nullopt : std::optional<int>(s.find(\"galaxy\"));";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestString, SubstrFunction) {
    std::string source = R"(
        import string;
        let s = "hello world";
        let sub = string::substr(s, 6, 5);
    )";
    std::string expected = "const std::string sub = s.substr(6, 5);";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}
