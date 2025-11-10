#include "TestHelpers.h"
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
// No extra include needed on Windows for system() exit code handling
#else
#include <sys/wait.h> // For WEXITSTATUS
#endif


std::string compile(const std::string& source, bool is_main_file) {
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();
    chtholly::Transpiler transpiler(is_main_file);
    return transpiler.transpile(statements);
}

std::string normalize(const std::string& s) {
    std::string result = s;
    result.erase(std::remove_if(result.begin(), result.end(),
                                [](unsigned char c) { return std::isspace(c); }),
                 result.end());
    return result;
}

void compile_and_expect_error(const std::string& source, const std::string& expected_error_message) {
    try {
        compile(source);
        FAIL() << "Expected std::runtime_error, but no exception was thrown.";
    } catch (const std::runtime_error& e) {
        std::string error_message = e.what();
        ASSERT_TRUE(error_message.find(expected_error_message) != std::string::npos)
            << "Expected error message to contain: " << expected_error_message
            << "\nBut got: " << error_message;
    }
}


int TranspilerTest::run(const std::string& source, bool is_main_file) {
    std::string cpp_code = ::compile(source, is_main_file);
    if (is_main_file) {
        cpp_code += "\nint main() { return chtholly_main(); }";
    }

    std::string temp_base = "temp_executable_" + std::to_string(rand());
    std::string temp_cpp_file = temp_base + ".cpp";
    std::ofstream out_cpp(temp_cpp_file);
    out_cpp << cpp_code;
    out_cpp.close();

    std::string executable_name = temp_base;
#ifdef _WIN32
    executable_name += ".exe";
#endif
    std::string compile_command = CXX_COMPILER;
    compile_command += " -std=c++17 " + temp_cpp_file + " -o " + executable_name;
    int compile_result = system(compile_command.c_str());

    if (compile_result != 0) {
        remove(temp_cpp_file.c_str());
        return -1;
    }

    int run_result = system(("./" + executable_name).c_str());

    remove(temp_cpp_file.c_str());
    remove(executable_name.c_str());

#ifdef _WIN32
    return run_result;
#else
    return WEXITSTATUS(run_result);
#endif
}

TranspilerTest::RunResult TranspilerTest::run_and_capture(const std::string& source, bool is_main_file) {
    std::string cpp_code = ::compile(source, is_main_file);
    if (is_main_file) {
        cpp_code += "\nint main() { return chtholly_main(); }";
    }

    std::string temp_base = "temp_executable_" + std::to_string(rand());
    std::string temp_cpp_file = temp_base + ".cpp";
    std::ofstream out_cpp(temp_cpp_file);
    out_cpp << cpp_code;
    out_cpp.close();

    std::string executable_name = temp_base;
#ifdef _WIN32
    executable_name += ".exe";
#endif
    std::string stderr_file = temp_base + "_stderr.log";
    std::string stdout_file = temp_base + "_stdout.log";
    std::string compile_command = CXX_COMPILER;
    compile_command += " -std=c++17 " + temp_cpp_file + " -o " + executable_name;
    int compile_result = system(compile_command.c_str());

    if (compile_result != 0) {
        remove(temp_cpp_file.c_str());
        return {-1, "COMPILATION_FAILED", ""};
    }

    std::string run_command = "./" + executable_name + " > " + stdout_file + " 2> " + stderr_file;
    int run_result = system(run_command.c_str());

    std::ifstream stderr_stream(stderr_file);
    std::string stderr_output((std::istreambuf_iterator<char>(stderr_stream)),
                             std::istreambuf_iterator<char>());

    std::ifstream stdout_stream(stdout_file);
    std::string stdout_output((std::istreambuf_iterator<char>(stdout_stream)),
                             std::istreambuf_iterator<char>());

    remove(temp_cpp_file.c_str());
    remove(executable_name.c_str());
    remove(stderr_file.c_str());
    remove(stdout_file.c_str());

#ifdef _WIN32
    return {run_result, stderr_output, stdout_output};
#else
    return {WEXITSTATUS(run_result), stderr_output, stdout_output};
#endif
}
