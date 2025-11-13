#include <gtest/gtest.h>
#include "TestHelpers.h"

class TestString : public ::testing::Test {};

TEST_F(TestString, LengthFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            print("hello".length());
            print("".length());
            let s = "test";
            print(s.length());
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "5\n0\n4\n");
}

TEST_F(TestString, SplitFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let s = "hello,world,chtholly";
            let parts = s.split(",");
            for (let part : parts) {
                print(part);
            }
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "hello\nworld\nchtholly\n");
}

TEST_F(TestString, FindFunctionFound) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let s = "hello world";
            let pos = s.find("world");
            print(pos.unwarp());
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "6\n");
}

TEST_F(TestString, FindFunctionNotFound) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let s = "hello world";
            let pos = s.find("galaxy");
            print(pos.is_none());
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\n");
}

TEST_F(TestString, SubstrFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let s = "hello world";
            print(s.substr(6, 5));
            print("chtholly".substr(0, 4));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "world\nchth\n");
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

TEST_F(TestString, ContainsFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let s = "hello world";
            print(s.contains("world"));
            print(s.contains("galaxy"));
            print(s.contains(""));
            print("".contains("a"));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\nfalse\ntrue\nfalse\n");
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
