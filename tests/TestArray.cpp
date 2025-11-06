#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(TestArray, DynamicArrayDeclaration) {
    std::string source = R"(
        func main() {
            let arr: array[int] = [1, 2, 3];
        }
    )";
    std::string expected = R"(#include <vector>
void main() {
const std::vector<int> arr = {1, 2, 3};
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestArray, FixedSizeArrayDeclaration) {
    std::string source = R"(
        func main() {
            let arr: array[int; 3] = [1, 2, 3];
        }
    )";
    std::string expected = R"(#include <array>
void main() {
const std::array<int, 3> arr = {1, 2, 3};
}
)";
    ASSERT_EQ(compile(source), expected);
}

TEST(TestArray, TypeInference) {
    std::string source = R"(
        func main() {
            let arr = [1, 2, 3];
        }
    )";
    std::string expected = R"(#include <vector>
void main() {
const std::vector<int> arr = {1, 2, 3};
}
)";
    ASSERT_EQ(compile(source), expected);
}
