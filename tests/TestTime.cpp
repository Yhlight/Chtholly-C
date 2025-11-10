#include <gtest/gtest.h>
#include "TestHelpers.h"

class TimeTest : public TranspilerTest {};

TEST_F(TimeTest, Now) {
    std::string code = R"(
        import time;
        func main() -> int {
            let t = time::now();
            return t > 0;
        }
    )";
    int result = run(code);
    ASSERT_EQ(result, 1);
}
