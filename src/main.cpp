#include "Chtholly.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    chtholly::Chtholly chtholly;
    std::vector<std::string> input_files;
    std::string output_file;
    std::string cxx_compiler;

    if (argc < 2) {
        chtholly.runPrompt();
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--cxx") {
            if (i + 1 < argc) {
                cxx_compiler = argv[++i];
            } else {
                std::cerr << "Error: --cxx option requires a value." << std::endl;
                return 1;
            }
        } else if (arg == "-o") {
            if (i + 1 < argc) {
                output_file = argv[++i];
            } else {
                std::cerr << "Error: -o option requires a value." << std::endl;
                return 1;
            }
        } else if (arg[0] == '-') {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            return 1;
        }
        else {
            input_files.push_back(arg);
        }
    }

    if (input_files.empty()) {
        chtholly.runPrompt();
        return 0;
    }

    // For backwards compatibility, if only one file is provided without options,
    // use the old execution path which runs the code immediately.
    if (input_files.size() == 1 && output_file.empty() && cxx_compiler.empty()) {
         return chtholly.runFile(argv[1]);
    }

    // Otherwise, call the new compilation-focused run method.
    // This method will be fully implemented in the next steps.
    return chtholly.run(input_files, output_file, cxx_compiler);
}
