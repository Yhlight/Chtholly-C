#include <gtest/gtest.h>
#include "TestHelpers.h"
#include <chrono>

class TestTime : public ::testing::Test {};

TEST_F(TestTime, NowFunction) {
    std::string source = R"(
        import time;
        import iostream;
        func main() -> int {
            let t = time::now();
            print(t);
            return 0;
        }
    )";
    chtholly::RunResult result = chtholly::run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    long long timestamp = std::stoll(result.stdout_output);
    long long current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    ASSERT_NEAR(timestamp, current_timestamp, 5); // Allow a 5-second difference
}
