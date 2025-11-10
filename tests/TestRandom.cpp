#include <gtest/gtest.h>
#include "TestHelpers.h"
#include <string>
#include <vector>

class TestRandom : public ::testing::Test {};

TEST_F(TestRandom, RandFunction) {
    std::string source = R"(
        import random;
        import iostream;
        func main() -> int {
            let r = random::rand();
            // We can't check the exact value, but we can check if it's a float.
            // This will fail to transpile for now.
            print(r);
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    // Just check that it prints something that looks like a float.
    ASSERT_TRUE(result.stdout_output.find('.') != std::string::npos);
}

TEST_F(TestRandom, RandintFunction) {
    std::string source = R"(
        import random;
        import iostream;
        func main() -> int {
            let r = random::randint(5, 10);
            print(r);
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    int value = std::stoi(result.stdout_output);
    ASSERT_GE(value, 5);
    ASSERT_LE(value, 10);
}
