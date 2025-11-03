#include <gtest/gtest.h>
#include "Chtholly.h"
#include "Error.h"
#include "AST/ASTPrinter.h"

// Test fixture for switch statement tests
class SwitchTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(SwitchTest, ParseSimpleSwitch) {
    std::string source = R"(
        let x = 1;
        switch (x) {
            case 1: {
                print("one");
                break;
            }
            case 2: {
                print("two");
                break;
            }
            default: {
                print("other");
            }
        }
    )";
    Chtholly chtholly;
    auto statements = chtholly.run(source, true);
    ASSERT_FALSE(ErrorReporter::hadError);
    ASSERT_EQ(statements.size(), 2);
    ASSERT_NE(dynamic_cast<LetStmt*>(statements[0].get()), nullptr);
    ASSERT_NE(dynamic_cast<SwitchStmt*>(statements[1].get()), nullptr);
}

TEST_F(SwitchTest, ResolveValidSwitch) {
    std::string source = R"(
        let x = 1;
        switch (x) {
            case 1: {
                print("one");
                break;
            }
            default: {
                print("other");
            }
        }
    )";
    Chtholly chtholly;
    chtholly.run(source, true);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(SwitchTest, ResolveBreakOutsideSwitch) {
    std::string source = "break;";
    Chtholly chtholly;
    chtholly.run(source, true);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(SwitchTest, ResolveFallthroughOutsideSwitch) {
    std::string source = "fallthrough;";
    Chtholly chtholly;
    chtholly.run(source, true);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(SwitchTest, TranspileSimpleSwitch) {
    std::string source = R"(
        let x = 1;
        switch (x) {
            case 1: {
                print("one");
                break;
            }
            default: {
                print("other");
            }
        }
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
    const double x = 1;
    {
        auto switch_val = x;
        bool matched = false;
        if (!matched && (switch_val == 1)) {
            matched = true;
        }
        if (matched)     {
    std::cout << "one" << std::endl;
    break;
    }
        if (!matched)     {
    std::cout << "other" << std::endl;
    }
    }
    return 0;
}
)";
    ASSERT_EQ(output, expected);
}

TEST_F(SwitchTest, TranspileSwitchWithFallthrough) {
    std::string source = R"(
        let x = 1;
        switch (x) {
            case 1: {
                print("one");
                fallthrough;
            }
            case 2: {
                print("two");
                break;
            }
        }
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
    const double x = 1;
    {
        auto switch_val = x;
        bool matched = false;
        if (!matched && (switch_val == 1)) {
            matched = true;
        }
        if (matched)     {
    std::cout << "one" << std::endl;
    }
        if (!matched && (switch_val == 2)) {
            matched = true;
        }
        if (matched)     {
    std::cout << "two" << std::endl;
    break;
    }
    }
    return 0;
}
)";
    ASSERT_EQ(output, expected);
}
