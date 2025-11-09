#include "Chtholly.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"

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

            chtholly::Transpiler transpiler(true);
            std::string output = transpiler.transpile(statements);

            // Add a C++ main function to call the Chtholly main and handle exit codes.
            output += R"(
#include <vector>
#include <string>

// Forward declare chtholly_main
int chtholly_main(std::vector<std::string> args);

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return chtholly_main(args);
}
)";

            std::ofstream out_file("_temp.cpp");
            out_file << "#include <string>\n" << output;
            out_file.close();

            // Compile the C++ code.
            int compile_status = system("g++ _temp.cpp -o _temp.out -std=c++17");
            if (compile_status != 0) {
                std::cerr << "C++ compilation failed." << std::endl;
                remove("_temp.cpp");
                return 1;
            }

            // Execute the compiled program.
            int exit_code = system("./_temp.out");

            // Clean up temporary files.
            remove("_temp.cpp");
            remove("_temp.out");

            return WEXITSTATUS(exit_code);

        } catch (const std::exception& e) {
            std::cerr << "An error occurred: " << e.what() << std::endl;
            return 1;
        }
        return 0;
    }
}
