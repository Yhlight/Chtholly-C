#include <gtest/gtest.h>
#include "TestHelpers.h"

class TestArrayStdLib : public ::testing::Test {};

TEST_F(TestArrayStdLib, LengthFunction) {
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

TEST_F(TestArrayStdLib, PopFunction) {
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

TEST_F(TestArrayStdLib, PushFunction) {
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
