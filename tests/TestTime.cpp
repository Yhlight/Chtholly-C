#include <gtest/gtest.h>
#include "TestHelpers.h"

TEST(TestTime, Now) {
    std::string code = R"(
        import time;
        func main() -> int {
            let t = time::now();
            return t > 0;
        }
    )";
    int result = chtholly::run(code);
    ASSERT_EQ(result, 1);
}
