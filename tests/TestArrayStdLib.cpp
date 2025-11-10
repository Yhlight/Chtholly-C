#include <gtest/gtest.h>
#include "TestHelpers.h"

class ArrayStdLibTest : public TranspilerTest {};

TEST_F(ArrayStdLibTest, LengthFunction) {
    std::string source = R"(
        import array;
        let arr = [1, 2, 3];
        let len = array::length(arr);
    )";
    std::string expected = "const int len = arr.size();";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(ArrayStdLibTest, ContainsFunction) {
    std::string source = R"(
        import array;
        import iostream;
        func main() -> int {
            let arr = [1, 2, 3];
            print(array::contains(arr, 2));
            print(array::contains(arr, 4));
            return 0;
        }
    )";
    RunResult result = run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "true\nfalse\n");
}

TEST_F(ArrayStdLibTest, ReverseFunction) {
    std::string source = R"(
        import array;
        import iostream;
        func main() -> int {
            mut arr = [1, 2, 3];
            array::reverse(arr);
            for (let i in arr) {
                print(i);
            }
            return 0;
        }
    )";
    RunResult result = run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stdout_output, "3\n2\n1\n");
}

TEST_F(ArrayStdLibTest, PopFunction) {
    std::string source = R"(
        import array;
        mut arr = [1, 2, 3];
        let last = array::pop(arr);
    )";
    std::string expected = "const int last = [&]() { auto val = arr.back(); arr.pop_back(); return val; }();";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}

TEST_F(ArrayStdLibTest, PushFunction) {
    std::string source = R"(
        import array;
        mut arr = [1, 2, 3];
        array::push(arr, 4);
    )";
    std::string expected = "arr.push_back(4);";
    std::string transpiled = compile(source);
    transpiled.erase(std::remove_if(transpiled.begin(), transpiled.end(), ::isspace), transpiled.end());
    expected.erase(std::remove_if(expected.begin(), expected.end(), ::isspace), expected.end());
    ASSERT_NE(transpiled.find(expected), std::string::npos);
}
