#include "cli.h"
#include <iostream>

namespace Chtholly {

CLIArgs parseArgs(int argc, char* argv[]) {
    if (argc != 2) {
        throw std::runtime_error("Invalid number of arguments");
    }
    return {argv[1]};
}

}
