#include <iostream>
#include "cli.h"

int main(int argc, char* argv[]) {
    try {
        Chtholly::CLIArgs args = Chtholly::parseArgs(argc, argv);
        std::cout << "Input file: " << args.inputFile << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
