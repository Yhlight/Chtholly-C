#include "Chtholly.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    std::string cxx_compiler;
    std::string file_path;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--cxx") {
            if (i + 1 < argc) {
                cxx_compiler = argv[++i];
            } else {
                std::cerr << "--cxx option requires one argument." << std::endl;
                return 1;
            }
        } else {
            if (!file_path.empty()) {
                std::cerr << "Usage: chtholly [--cxx <compiler>] [script]" << std::endl;
                return 64;
            }
            file_path = arg;
        }
    }

    chtholly::Chtholly chtholly_instance(cxx_compiler);

    if (!file_path.empty()) {
        return chtholly_instance.runFile(file_path);
    } else {
        chtholly_instance.runPrompt();
    }

    return 0;
}
