#include <gtest/gtest.h>
#include <cstdlib>
#include <fstream>
#include <string>

#ifndef _WIN32
#include <sys/wait.h>
#endif
#include <string>

// Helper to create a temporary file
void create_temp_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

TEST(TestExecution, MainReturnsZero) {
    std::string filename = "_test_pass.cns";
    std::string source = R"(
        func main(args: array[string]) -> int {
            return 0;
        }
    )";
    create_temp_file(filename, source);

    // The test executable is in build/tests, the main executable is in build/src
    std::string command = "../src/chtholly " + filename;
    int exit_code = std::system(command.c_str());

    remove(filename.c_str());

#ifdef _WIN32
    ASSERT_EQ(exit_code, 0);
#else
    ASSERT_EQ(WEXITSTATUS(exit_code), 0);
#endif
}

TEST(TestExecution, MainReturnsOne) {
    std::string filename = "_test_fail.cns";
    std::string source = R"(
        func main(args: array[string]) -> int {
            return 1;
        }
    )";
    create_temp_file(filename, source);

    // The test executable is in build/tests, the main executable is in build/src
    std::string command = "../src/chtholly " + filename;
    int exit_code = std::system(command.c_str());

    remove(filename.c_str());

#ifdef _WIN32
    ASSERT_EQ(exit_code, 1);
#else
    ASSERT_EQ(WEXITSTATUS(exit_code), 1);
#endif
}
