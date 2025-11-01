#include "gtest/gtest.h"
#include "TestHelpers.h"
#include <string>
#include <vector>

TEST(IfElseTest, SimpleIf) {
    std::string code = R"(
        if (true) {
            let a = 1;
        }
    )";
    std::string expected = R"(
        if (true) {
            const auto a = 1;
        }
    )";
    EXPECT_EQ(normalize(compile(code)), normalize(expected));
}

TEST(IfElseTest, IfElse) {
    std::string code = R"(
        if (false) {
            let a = 1;
        } else {
            let b = 2;
        }
    )";
    std::string expected = R"(
        if (false) {
            const auto a = 1;
        } else {
            const auto b = 2;
        }
    )";
    EXPECT_EQ(normalize(compile(code)), normalize(expected));
}

TEST(IfElseTest, NestedIf) {
    std::string code = R"(
        if (true) {
            if (false) {
                let a = 1;
            }
        }
    )";
    std::string expected = R"(
        if (true) {
            if (false) {
                const auto a = 1;
            }
        }
    )";
    EXPECT_EQ(normalize(compile(code)), normalize(expected));
}

TEST(IfElseTest, IfElseIf) {
    std::string code = R"(
        if (false) {
            let a = 1;
        } else if (true) {
            let b = 2;
        } else {
            let c = 3;
        }
    )";
    std::string expected = R"(
        if (false) {
            const auto a = 1;
        } else if (true) {
            const auto b = 2;
        } else {
            const auto c = 3;
        }
    )";
    EXPECT_EQ(normalize(compile(code)), normalize(expected));
}
