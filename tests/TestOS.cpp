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

class OSTest : public TranspilerTest {};

TEST_F(OSTest, ExitFunction) {
    std::string source = R"(
        import os;
        func main() -> int {
            os::exit(42);
            return 0; // Should not be reached
        }
    )";
    int exit_code = run(source, true);
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

    RunResult result = run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stderr_output, "");
    ASSERT_EQ(result.stdout_output, "HelloWorld\n");

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

    RunResult result = run_and_capture(source, true);
    ASSERT_EQ(result.exit_code, 0);
    ASSERT_EQ(result.stderr_output, "");
    ASSERT_EQ(result.stdout_output, "DEFAULT_VALUE\n");
}
