#include <gtest/gtest.h>
#include "TestHelpers.h"

class RandomTest : public TranspilerTest {};

TEST_F(RandomTest, RandFunction) {
    std::string code = R"(
        import random;
        func main() -> int {
            let r = random::rand();
            // We can't test for a specific value, but we can check if it's in range.
            // For now, just successfully calling it is a pass.
            // A more complex test could be added later to check the range.
            return 0;
        }
    )";
    EXPECT_EQ(run(code), 0);
}

TEST_F(RandomTest, RandintFunction) {
    std::string code = R"(
        import random;
        import iostream;
        func main() -> int {
            let r = random::randint(1, 10);
            print(r);
            return 0;
        }
    )";
    RunResult result = run_and_capture(code);
    ASSERT_EQ(result.exit_code, 0);
    int random_val = std::stoi(result.stdout_output);
    ASSERT_GE(random_val, 1);
    ASSERT_LE(random_val, 10);
}
