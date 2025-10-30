#include <gtest/gtest.h>
#include "cli.h"

TEST(CLI, ValidArguments) {
    char* argv[] = {"chtholly", "test.cns", nullptr};
    int argc = 2;
    Chtholly::CLIArgs args = Chtholly::parseArgs(argc, argv);
    ASSERT_EQ(args.inputFile, "test.cns");
}

TEST(CLI, InvalidArguments) {
    char* argv[] = {"chtholly", nullptr};
    int argc = 1;
    ASSERT_THROW(Chtholly::parseArgs(argc, argv), std::runtime_error);
}
