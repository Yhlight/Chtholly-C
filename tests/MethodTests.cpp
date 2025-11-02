#include "gtest/gtest.h"
#include "Chtholly.h"
#include "Error.h"

class MethodTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(MethodTest, Declaration) {
    std::string source = R"(
        struct Point {
            func getX() {
                return self.x;
            }
            let x = 0;
        }
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(MethodTest, Call) {
    std::string source = R"(
        struct Point {
            func getX() {
                return self.x;
            }
            let x = 1;
        }
        let p = Point();
        print(p.getX());
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(MethodTest, ThisOutsideStruct) {
    std::string source = "print(self);";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_TRUE(ErrorReporter::hadError);
}
