#include <gtest/gtest.h>
#include "TestHelpers.h"

class TestArrayStdLib : public ::testing::Test {};

TEST_F(TestArrayStdLib, LengthFunction) {
    std::string source = R"(
        let arr = [1, 2, 3];
        let len = arr.length();
    )";
    std::string expected = "const int len = arr.size();";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestArrayStdLib, ContainsFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            let arr = [1, 2, 3];
            print(arr.contains(2));
            print(arr.contains(4));
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\nfalse\n");
}

TEST_F(TestArrayStdLib, ReverseFunction) {
    std::string source = R"(
        import iostream;
        func main() -> int {
            mut arr = [1, 2, 3];
            arr.reverse();
            for (let i in arr) {
                print(i);
            }
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "3\n2\n1\n");
}

TEST_F(TestArrayStdLib, PopFunction) {
    std::string source = R"(
        mut arr = [1, 2, 3];
        let last = arr.pop();
    )";
    std::string expected = "const int last = [&]() { auto val = arr.back(); arr.pop_back(); return val; }();";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(TestArrayStdLib, PushFunction) {
    std::string source = R"(
        mut arr = [1, 2, 3];
        arr.push(4);
    )";
    std::string expected = "arr.push_back(4);";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}
