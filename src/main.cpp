#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

int main() {
    llvm::LLVMContext context;
    llvm::Module module("Chtholly", context);

    std::cout << "Hello, Chtholly!" << std::endl;
    std::cout << "Successfully initialized LLVM." << std::endl;

    std::string source = "let x = 10;";
    Lexer lexer(source);
    Parser parser(lexer);

    try {
        auto ast = parser.parse_expression();
        std::cout << "Parsed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing: " << e.what() << std::endl;
    }

    return 0;
}
