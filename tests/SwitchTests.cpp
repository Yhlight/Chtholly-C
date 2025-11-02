#include "gtest/gtest.h"
#include "Chtholly.h"
#include "Error.h"

class SwitchTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(SwitchTest, BasicSwitch) {
    std::string source = R"(
        let a = 1;
        switch (a) {
            case 1: {
                print("one");
                break;
            }
            case 2: {
                print("two");
                break;
            }
        }
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(SwitchTest, DefaultCase) {
    std::string source = R"(
        let a = 3;
        switch (a) {
            case 1: {
                print("one");
                break;
            }
            default: {
                print("default");
                break;
            }
        }
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(SwitchTest, Fallthrough) {
    std::string source = R"(
        let a = 1;
        switch (a) {
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
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(SwitchTest, ExpressionCase) {
    std::string source = R"(
        let a = 1;
        switch (a) {
            case 1 + 0: {
                print("one");
                break;
            }
        }
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(SwitchTest, MisplacedBreak) {
    std::string source = "break;";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(SwitchTest, MisplacedFallthrough) {
    std::string source = "fallthrough;";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_TRUE(ErrorReporter::hadError);
}
