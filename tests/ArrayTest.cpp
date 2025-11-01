#include "TestHelpers.h"
#include <gtest/gtest.h>

const std::string PREAMBLE = "#include <string>\n#include <vector>\n#include <array>\n";

TEST(ArrayTest, DynamicArrayInitialization) {
    std::string source = R"(
        let my_array: [int] = [1, 2, 3];
    )";
    std::string expected = PREAMBLE + R"(
        const std::vector<int> my_array = {1, 2, 3};
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(ArrayTest, FixedSizeArrayInitialization) {
    std::string source = R"(
        let my_array: [int, 3] = [1, 2, 3];
    )";
    std::string expected = PREAMBLE + R"(
        const std::array<int, 3> my_array = {1, 2, 3};
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(ArrayTest, ArrayAccess) {
    std::string source = R"(
        let my_array: [int] = [1, 2, 3];
        let x = my_array[1];
    )";
    std::string expected = PREAMBLE + R"(
        const std::vector<int> my_array = {1, 2, 3};
        const int x = my_array[1];
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}

TEST(ArrayTest, TypeInference) {
    std::string source = R"(
        let my_array = [1, 2, 3];
    )";
    std::string expected = PREAMBLE + R"(
        const std::vector<int> my_array = {1, 2, 3};
    )";
    std::string result = compile(source);
    ASSERT_EQ(normalize(result), normalize(expected));
}
