#include "Chtholly.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#ifndef _WIN32
#include <sys/wait.h>
#endif
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"
#include "Transpiler.h"
#include <filesystem>

namespace chtholly {
    int Chtholly::runFile(const std::string &path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << path << std::endl;
            return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        return run(source);
    }

    int Chtholly::run(const std::vector<std::string>& files, const std::string& output_file, const std::string& cxx_compiler_path) {
        std::vector<std::string> temp_files;
        bool main_found = false;

        for (size_t i = 0; i < files.size(); ++i) {
            std::ifstream file(files[i]);
            if (!file.is_open()) {
                std::cerr << "Error: Could not open file " << files[i] << std::endl;
                return 1;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string source = buffer.str();

            try {
                chtholly::Lexer lexer(source);
                std::vector<chtholly::Token> tokens = lexer.scanTokens();

                chtholly::Parser parser(tokens);
                std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();

                chtholly::Resolver resolver;
                resolver.resolve(statements);

                if (resolver.hadError) {
                    return 1;
                }

                bool is_main = false;
                for (const auto& stmt : statements) {
                    if (auto* func = dynamic_cast<FunctionStmt*>(stmt.get())) {
                        if (func->name.lexeme == "main") {
                             if (main_found) {
                                std::cerr << "Error: Multiple main functions found. Only one is allowed." << std::endl;
                                return 1;
                            }
                            is_main = true;
                            main_found = true;
                        }
                    }
                }

                chtholly::Transpiler transpiler(is_main);
                std::string output = transpiler.transpile(statements);

                std::string temp_filename = "_temp_" + std::to_string(i) + ".cpp";
                std::ofstream out_file(temp_filename);
                out_file << output;
                out_file.close();
                temp_files.push_back(temp_filename);

            } catch (const std::exception& e) {
                std::cerr << "An error occurred while processing " << files[i] << ": " << e.what() << std::endl;
                 // Clean up any files we've created so far
                for (const auto& temp_file : temp_files) {
                    remove(temp_file.c_str());
                }
                return 1;
            }
        }

        if (!main_found) {
            std::cerr << "Error: No main function found in any of the input files." << std::endl;
            // Clean up any files we've created so far
            for (const auto& temp_file : temp_files) {
                remove(temp_file.c_str());
            }
            return 1;
        }

        std::string compiler = cxx_compiler_path.empty() ? CXX_COMPILER : cxx_compiler_path;
#ifdef _WIN32
        std::string output = output_file.empty() ? "a.exe" : output_file;
#else
        std::string output = output_file.empty() ? "a.out" : output_file;
#endif

        std::string compile_command = compiler + " -std=c++17 -o " + output;
        for (const auto& temp_file : temp_files) {
            compile_command += " " + temp_file;
        }

        int compile_status = system(compile_command.c_str());

        for (const auto& temp_file : temp_files) {
            remove(temp_file.c_str());
        }

        if (compile_status != 0) {
            std::cerr << "C++ compilation failed." << std::endl;
            return 1;
        }

        return 0;
    }

    void Chtholly::runPrompt() {
        std::cout << "> ";
        std::string line;
        while (std::getline(std::cin, line)) {
            run(line);
            std::cout << "> ";
        }
    }

    int Chtholly::run(const std::string &source) {
        try {
            chtholly::Lexer lexer(source);
            std::vector<chtholly::Token> tokens = lexer.scanTokens();

            chtholly::Parser parser(tokens);
            std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();

            chtholly::Resolver resolver;
            resolver.resolve(statements);

            if (resolver.hadError) {
                return 1;
            }

            chtholly::Transpiler transpiler(true);
            std::string output = transpiler.transpile(statements);

            std::string temp_base = "temp_executable_" + std::to_string(rand());
            std::string temp_cpp = temp_base + ".cpp";
            std::string temp_out = temp_base;

#ifdef _WIN32
            temp_out += ".exe";
#endif

            std::ofstream out_file(temp_cpp);
            out_file << output;
            out_file.close();

            // Compile the C++ code.
            std::string command = std::string(CXX_COMPILER) + " " + temp_cpp + " -o " + temp_out + " -std=c++17";
            int compile_status = system(command.c_str());
            if (compile_status != 0) {
                std::cerr << "C++ compilation failed." << std::endl;
                remove(temp_cpp.c_str());
                return 1;
            }

            // Execute the compiled program.
#ifdef _WIN32
            int exit_code = system(temp_out.c_str());
#else
            int exit_code = system(("./" + temp_out).c_str());
#endif

            // Clean up temporary files.
            remove(temp_cpp.c_str());
            remove(temp_out.c_str());

#ifdef _WIN32
            return exit_code;
#else
            return WEXITSTATUS(exit_code);
#endif

        } catch (const std::exception& e) {
            std::cerr << "An error occurred: " << e.what() << std::endl;
            return 1;
        }
        return 0;
    }
}
