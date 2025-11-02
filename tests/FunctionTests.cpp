#include "gtest/gtest.h"
#include "Chtholly.h"
#include "Error.h"

class FunctionTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(FunctionTest, Declaration) {
    std::string source = R"(
        func add(a, b) {
            return a + b;
        }
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(FunctionTest, Call) {
    std::string source = R"(
        func add(a, b) {
            return a + b;
        }
        print(add(1, 2));
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(FunctionTest, ReturnValue) {
    std::string source = R"(
        func identity(a) {
            return a;
        }
        print(identity(5));
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(FunctionTest, ReturnVoid) {
    std::string source = R"(
        func void_func() {
            return;
        }
        void_func();
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(FunctionTest, MisplacedReturn) {
    std::string source = "return 1;";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_TRUE(ErrorReporter::hadError);
}
