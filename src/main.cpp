#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"

int main() {
    std::string source = "let a = 10;";
    Chtholly::Lexer lexer(source);
    Chtholly::Parser parser(lexer);
    auto program = parser.parseProgram();

    Chtholly::CodeGen codegen;
    std::string ir;
    codegen.generate(*program, ir);

    std::cout << ir << std::endl;

    return 0;
}
