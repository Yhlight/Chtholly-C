#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(TestEnum, EnumDeclaration) {
    std::string source = R"(
        enum Color {
            Red,
            Green,
            Blue
        }

        func main() {
            let my_color: Color = Color::Red;
        }
    )";
    std::string expected = R"(enum class Color {
    Red,
    Green,
    Blue
};
void main() {
const Color my_color = Color::Red;
}
)";
    ASSERT_EQ(compile(source), expected);
}
