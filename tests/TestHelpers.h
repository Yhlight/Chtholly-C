#ifndef CHTHOLLY_TEST_HELPERS_H
#define CHTHOLLY_TEST_HELPERS_H

#include <string>

// Transpiles a string of Chtholly source code to C++.
std::string compile(const std::string& source);

// Removes whitespace and newlines to allow for more flexible string comparison.
std::string normalize(const std::string& s);

#endif // CHTHOLLY_TEST_HELPERS_H
