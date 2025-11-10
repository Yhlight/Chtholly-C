#include <gtest/gtest.h>
#include "TestHelpers.h"

class TestString : public ::testing::Test {};

TEST_F(TestString, LengthFunction) {
    std::string source = R"(
        let s = "hello";
        let len = s.length();
    )";
    std::string expected = "const int len = s.length();";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestString, SplitFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let s = "hello,world,chtholly";
            let parts = s.split(",");
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
        let s = "hello world";
        let pos = s.find("world");
    )";
    std::string expected = "const std::optional<int> pos = (s.find(\"world\") == std::string::npos) ? std::nullopt : std::optional<int>(s.find(\"world\"));";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestString, FindFunctionNotFound) {
    std::string source = R"(
        let s = "hello world";
        let pos = s.find("galaxy");
    )";
    std::string expected = "const std::optional<int> pos = (s.find(\"galaxy\") == std::string::npos) ? std::nullopt : std::optional<int>(s.find(\"galaxy\"));";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestString, SubstrFunction) {
    std::string source = R"(
        let s = "hello world";
        let sub = s.substr(6, 5);
    )";
    std::string expected = "const std::string sub = s.substr(6, 5);";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestString, ToUpperFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            print("hello world".to_upper());
            print("".to_upper());
            let s = "ALREADY UPPER";
            print(s.to_upper());
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "HELLO WORLD\n\nALREADY UPPER\n");
}

TEST_F(TestString, ToLowerFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            print("HELLO WORLD".to_lower());
            print("".to_lower());
            let s = "already lower";
            print(s.to_lower());
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "hello world\n\nalready lower\n");
}

TEST_F(TestString, TrimFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            print("  hello world  ".trim());
            print("no spaces".trim());
            print("".trim());
            print("   ".trim());
            print(" leading".trim());
            print("trailing  ".trim());
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "hello world\nno spaces\n\n\nleading\ntrailing\n");
}

TEST_F(TestString, StartsWithFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let s = "hello world";
            print(s.starts_with("hello"));
            print(s.starts_with("world"));
            print(s.starts_with(""));
            print("".starts_with("a"));
            print("".starts_with(""));
            print(s.starts_with("hello world"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\nfalse\ntrue\nfalse\ntrue\ntrue\n");
}

TEST_F(TestString, IsEmptyFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let s1 = "";
            print(s1.is_empty());
            let s2 = "hello";
            print(s2.is_empty());
            print("".is_empty());
            print("  ".is_empty());
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\nfalse\ntrue\nfalse\n");
}

TEST_F(TestString, ReplaceFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let s1 = "hello world, hello chtholly";
            print(s1.replace("hello", "hi"));
            let s2 = "no change";
            print(s2.replace("foo", "bar"));
            let s3 = "aaaaa";
            print(s3.replace("a", "b"));
            print("".replace("a", "b"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "hi world, hi chtholly\nno change\nbbbbb\n\n");
}

TEST_F(TestString, EndsWithFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let s = "hello world";
            print(s.ends_with("world"));
            print(s.ends_with("hello"));
            print(s.ends_with(""));
            print("".ends_with("a"));
            print("".ends_with(""));
            print(s.ends_with("hello world"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\nfalse\ntrue\nfalse\ntrue\ntrue\n");
}
