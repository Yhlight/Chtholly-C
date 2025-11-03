#include <gtest/gtest.h>
#include "Chtholly.h"
#include "Error.h"

// Test fixture for meta module tests
class MetaTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(MetaTest, IsStruct) {
    std::string source = R"(
        struct MyStruct {}
        let is_s = meta.is_struct<MyStruct>();
        let is_not_s = meta.is_struct<int>();
    )";
    Chtholly chtholly;
    std::string output = chtholly.run(source);
    std::string expected = R"(#include <iostream>
#include <variant>
#include <string>

std::string chtholly_input() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

struct MyStruct {
};

int main() {
    const bool is_s = true;
    const bool is_not_s = false;
    return 0;
}
)";
    ASSERT_EQ(output, expected);
}

TEST_F(MetaTest, IsInt) {
    std::string source = R"(
        let is_i = meta.is_int<int>();
        let is_not_i = meta.is_int<string>();
    )";
    Chtholly chtholly;
    std::string output = chtholly.run(source);
    std::string expected = R"(#include <iostream>
#include <variant>
#include <string>

std::string chtholly_input() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

int main() {
    const bool is_i = true;
    const bool is_not_i = false;
    return 0;
}
)";
    ASSERT_EQ(output, expected);
}
