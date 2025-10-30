#include <iostream>
#include <fstream>
#include <sstream>
#include "cli.h"
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

int main(int argc, char* argv[]) {
    try {
        Chtholly::CLIArgs args = Chtholly::parseArgs(argc, argv);

        std::ifstream file(args.inputFile);
        if (!file) {
            std::cerr << "Error: could not open file " << args.inputFile << std::endl;
            return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();

        Chtholly::Lexer l(source);
        Chtholly::Parser p(l);
        auto program = p.parseProgram();

        if (!p.errors().empty()) {
            std::cerr << "Parser has " << p.errors().size() << " errors" << std::endl;
            for (const auto& msg : p.errors()) {
                std::cerr << "parser error: " << msg << std::endl;
            }
            return 1;
        }

        Chtholly::CodeGen c;
        std::string generated_code = c.generate(program.get());

        std::cout << generated_code << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
