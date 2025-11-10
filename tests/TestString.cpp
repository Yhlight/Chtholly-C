#include <gtest/gtest.h>
#include "TestHelpers.h"

class TestString : public ::testing::Test {};

TEST_F(TestString, LengthFunction) {
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

TEST_F(TestString, SplitFunction) {
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
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "hello\nworld\nchtholly\n");
}

TEST_F(TestString, JoinFunction) {
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
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "hello,world,chtholly\n");
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

TEST_F(TestString, ToUpperFunction) {
    std::string source = R"(
        import string;
        import iostream;
        func main() -> int {
            print(string::to_upper("hello world"));
            print(string::to_upper(""));
            print(string::to_upper("ALREADY UPPER"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "HELLO WORLD\n\nALREADY UPPER\n");
}

TEST_F(TestString, ToLowerFunction) {
    std::string source = R"(
        import string;
        import iostream;
        func main() -> int {
            print(string::to_lower("HELLO WORLD"));
            print(string::to_lower(""));
            print(string::to_lower("already lower"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "hello world\n\nalready lower\n");
}

TEST_F(TestString, TrimFunction) {
    std::string source = R"(
        import string;
        import iostream;
        func main() -> int {
            print(string::trim("  hello world  "));
            print(string::trim("no spaces"));
            print(string::trim(""));
            print(string::trim("   "));
            print(string::trim(" leading"));
            print(string::trim("trailing  "));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "hello world\nno spaces\n\n\nleading\ntrailing\n");
}

TEST_F(TestString, StartsWithFunction) {
    std::string source = R"(
        import string;
        import iostream;
        func main() -> int {
            let s = "hello world";
            print(string::starts_with(s, "hello"));
            print(string::starts_with(s, "world"));
            print(string::starts_with(s, ""));
            print(string::starts_with("", "a"));
            print(string::starts_with("", ""));
            print(string::starts_with(s, "hello world"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\nfalse\ntrue\nfalse\ntrue\ntrue\n");
}

TEST_F(TestString, EndsWithFunction) {
    std::string source = R"(
        import string;
        import iostream;
        func main() -> int {
            let s = "hello world";
            print(string::ends_with(s, "world"));
            print(string::ends_with(s, "hello"));
            print(string::ends_with(s, ""));
            print(string::ends_with("", "a"));
            print(string::ends_with("", ""));
            print(string::ends_with(s, "hello world"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\nfalse\ntrue\nfalse\ntrue\ntrue\n");
}
