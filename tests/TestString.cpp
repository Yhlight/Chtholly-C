#include <gtest/gtest.h>
#include "TestHelpers.h"

class StringTest : public TranspilerTest {};

TEST_F(StringTest, LengthFunction) {
    std::string source = R"(
        import string;
        let s = "hello";
        let len = string::length(s);
    )";
    std::string expected = "const int len = s.length();";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(StringTest, SplitFunction) {
    std::string source = R"(
        import string;
        import iostream;
        func main() -> int {
            let s = "hello,world,chtholly";
            let parts = string::split(s, ",");
            for (let part in parts) {
                print(part);
            }
            return 0;
        }
    )";
    RunResult result = run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "hello\nworld\nchtholly\n");
}

TEST_F(StringTest, JoinFunction) {
    std::string source = R"(
        import string;
        import iostream;
        func main() -> int {
            let arr = ["hello", "world", "chtholly"];
            let s = string::join(arr, ",");
            print(s);
            return 0;
        }
    )";
    RunResult result = run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "hello,world,chtholly\n");
}

TEST_F(StringTest, FindFunctionFound) {
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

TEST_F(StringTest, FindFunctionNotFound) {
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

TEST_F(StringTest, SubstrFunction) {
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
