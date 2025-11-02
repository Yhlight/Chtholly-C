#include "gtest/gtest.h"
#include "Chtholly.h"
#include "Error.h"

class StructTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(StructTest, Declaration) {
    std::string source = R"(
        struct Point {
            let x = 0;
            let y = 0;
        }
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(StructTest, Instantiation) {
    std::string source = R"(
        struct Point {
            let x = 0;
            let y = 0;
        }
        let p = Point();
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(StructTest, GetField) {
    std::string source = R"(
        struct Point {
            let x = 1;
            let y = 2;
        }
        let p = Point();
        print(p.x);
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}

TEST_F(StructTest, SetField) {
    std::string source = R"(
        struct Point {
            mut x = 1;
            let y = 2;
        }
        let p = Point();
        p.x = 3;
        print(p.x);
    )";
    Chtholly chtholly;
    chtholly.run(source);
    ASSERT_FALSE(ErrorReporter::hadError);
}
