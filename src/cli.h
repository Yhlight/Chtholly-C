#ifndef CHTHOLLY_CLI_H
#define CHTHOLLY_CLI_H

#include <string>
#include <vector>

namespace Chtholly {
    struct CLIArgs {
        std::string inputFile;
    };
    CLIArgs parseArgs(int argc, char* argv[]);
}

#endif // CHTHOLLY_CLI_H
