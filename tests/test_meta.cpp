#include <gtest/gtest.h>
#include "../src/driver/Chtholly.h"
#include "../src/common/Error.h"

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

TEST_F(MetaTest, IsUint) {
    std::string source = R"(
        let is_u = meta.is_uint<uint>();
        let is_not_u = meta.is_uint<int>();
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
    const bool is_u = true;
    const bool is_not_u = false;
    return 0;
}
)";
    ASSERT_EQ(output, expected);
}

TEST_F(MetaTest, IsDouble) {
    std::string source = R"(
        let is_d = meta.is_double<double>();
        let is_not_d = meta.is_double<int>();
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
    const bool is_d = true;
    const bool is_not_d = false;
    return 0;
}
)";
    ASSERT_EQ(output, expected);
}

TEST_F(MetaTest, IsChar) {
    std::string source = R"(
        let is_c = meta.is_char<char>();
        let is_not_c = meta.is_char<int>();
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
    const bool is_c = true;
    const bool is_not_c = false;
    return 0;
}
)";
    ASSERT_EQ(output, expected);
}

TEST_F(MetaTest, IsBool) {
    std::string source = R"(
        let is_b = meta.is_bool<bool>();
        let is_not_b = meta.is_bool<int>();
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
    const bool is_b = true;
    const bool is_not_b = false;
    return 0;
}
)";
    ASSERT_EQ(output, expected);
}

TEST_F(MetaTest, IsString) {
    std::string source = R"(
        let is_s = meta.is_string<string>();
        let is_not_s = meta.is_string<int>();
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
    const bool is_s = true;
    const bool is_not_s = false;
    return 0;
}
)";
    ASSERT_EQ(output, expected);
}

TEST_F(MetaTest, IsArray) {
    std::string source = R"(
        let is_a = meta.is_array<array>();
        let is_not_a = meta.is_array<int>();
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
    const bool is_a = true;
    const bool is_not_a = false;
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
