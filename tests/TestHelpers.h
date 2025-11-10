#ifndef CHTHOLLY_TEST_HELPERS_H
#define CHTHOLLY_TEST_HELPERS_H

#include <string>

// Transpiles a string of Chtholly source code to C++.
std::string compile(const std::string& source, bool is_main_file = false);

// Removes whitespace and newlines to allow for more flexible string comparison.
std::string normalize(const std::string& s);

// Compiles a string of Chtholly source code and expects a std::runtime_error.
void compile_and_expect_error(const std::string& source, const std::string& expected_error_message);

namespace chtholly {
    int run(const std::string& source, bool is_main_file = true);
}

#endif // CHTHOLLY_TEST_HELPERS_H
