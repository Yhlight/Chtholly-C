#include <gtest/gtest.h>
#include "Error.h"

struct ErrorReporterTest : testing::Test {
    void SetUp() override {
        ErrorReporter::reset();
    }
};

TEST_F(ErrorReporterTest, ReportsErrorAndSetsFlag) {
    ASSERT_FALSE(ErrorReporter::hadError);
    ErrorReporter::error(1, "Test error");
    ASSERT_TRUE(ErrorReporter::hadError);
}

TEST_F(ErrorReporterTest, ResetClearsFlag) {
    ErrorReporter::error(1, "Test error");
    ASSERT_TRUE(ErrorReporter::hadError);
    ErrorReporter::reset();
    ASSERT_FALSE(ErrorReporter::hadError);
}
