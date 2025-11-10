#include <gtest/gtest.h>
#include "TestHelpers.h"
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <string>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

// Helper to set environment variables portably
void set_test_env(const char* name, const char* value) {
#ifdef _WIN32
    std::string env_string = std::string(name) + "=" + value;
    _putenv(env_string.c_str());
#else
    setenv(name, value, 1);
#endif
}

// Helper to unset environment variables portably
void unset_test_env(const char* name) {
#ifdef _WIN32
    std::string env_string = std::string(name) + "=";
    _putenv(env_string.c_str());
#else
    unsetenv(name);
#endif
}

// Helper function to compile and run C++ code, capturing its output.
std::string compile_and_run_cpp(const std::string& cpp_source, const std::string& test_name) {
    std::string temp_cpp_file = test_name + ".cpp";
    std::string temp_executable;
#ifdef _WIN32
    temp_executable = test_name + ".exe";
#else
    temp_executable = "./" + test_name;
#endif
    std::string temp_output_file = test_name + "_output.txt";

    std::ofstream out(temp_cpp_file);
    out << cpp_source;
    out.close();

    std::string compile_command = CXX_COMPILER;
    compile_command += " " + temp_cpp_file + " -o " + temp_executable + " -std=c++17";
    int compile_result = std::system(compile_command.c_str());
    if (compile_result != 0) {
        remove(temp_cpp_file.c_str());
        return "COMPILATION_FAILED";
    }

    std::string run_command = temp_executable + " > " + temp_output_file;
    std::system(run_command.c_str());

    std::ifstream output_file(temp_output_file);
    std::stringstream buffer;
    buffer << output_file.rdbuf();
    output_file.close();

    remove(temp_cpp_file.c_str());
    remove(temp_executable.c_str());
    remove(temp_output_file.c_str());

    return buffer.str();
}


class OSTest : public ::testing::Test {};

TEST_F(OSTest, ExitFunction) {
    std::string source = R"(
        import os;
        func main() -> int {
            os::exit(42);
            return 0; // Should not be reached
        }
    )";
    int exit_code = chtholly::run(source, true);
    ASSERT_EQ(exit_code, 42);
}

TEST_F(OSTest, EnvFunctionFound) {
    set_test_env("CHTHOLLY_TEST_VAR", "HelloWorld");

    std::string source = R"(
        import os;
        import iostream;
        func main() -> int {
            let val = os::env("CHTHOLLY_TEST_VAR");
            print(val.unwarp_or("NOT_FOUND"));
            return 0;
        }
    )";

    std::string cpp_code = compile(source);
    std::string output = compile_and_run_cpp(cpp_code, "test_env_found");

    ASSERT_EQ(output, "HelloWorld\n");

    unset_test_env("CHTHOLLY_TEST_VAR");
}

TEST_F(OSTest, EnvFunctionNotFound) {
    unset_test_env("CHTHOLLY_TEST_VAR_MISSING"); // Make sure it's not set

    std::string source = R"(
        import os;
        import iostream;
        func main() -> int {
            let val = os::env("CHTHOLLY_TEST_VAR_MISSING");
            print(val.unwarp_or("DEFAULT_VALUE"));
            return 0;
        }
    )";

    std::string cpp_code = compile(source);
    std::string output = compile_and_run_cpp(cpp_code, "test_env_not_found");

    ASSERT_EQ(output, "DEFAULT_VALUE\n");
}
