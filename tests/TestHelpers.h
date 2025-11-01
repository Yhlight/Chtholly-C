#pragma once

#include <string>

// Compiles a string of Chtholly code and returns the generated C++ code.
std::string compile(const std::string& source);

// Normalizes a string by removing leading/trailing whitespace and collapsing
// multiple whitespace characters into a single space.
std::string normalize(const std::string& s);
