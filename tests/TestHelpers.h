#ifndef CHTHOLLY_TEST_HELPERS_H
#define CHTHOLLY_TEST_HELPERS_H

#include <gtest/gtest.h>
#include <string>

// A test fixture for tests that need to run the transpiler.
class TranspilerTest : public ::testing::Test {
protected:
    int run(const std::string& source, bool is_main_file = true);
    struct RunResult {
        int exit_code;
        std::string stderr_output;
        std::string stdout_output;
    };
    RunResult run_and_capture(const std::string& source, bool is_main_file = true);
};

// Transpiles a string of Chtholly source code to C++.
std::string compile(const std::string& source, bool is_main_file = false);

// Removes whitespace and newlines to allow for more flexible string comparison.
std::string normalize(const std::string& s);

// Compiles a string of Chtholly source code and expects a std::runtime_error.
void compile_and_expect_error(const std::string& source, const std::string& expected_error_message);

#endif // CHTHOLLY_TEST_HELPERS_H
