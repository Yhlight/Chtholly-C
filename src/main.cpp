#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "Sema.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

int main() {
    llvm::LLVMContext context;
    llvm::Module module("Chtholly", context);

    std::cout << "Hello, Chtholly!" << std::endl;
    std::cout << "Successfully initialized LLVM." << std::endl;

    std::string source = "let x = 10; let y = x; let z = x;";
    Lexer lexer(source);
    Parser parser(lexer);
    Sema sema;

    try {
        auto ast = parser.parse_block();
        std::cout << "Parsed successfully!" << std::endl;
        sema.visit(*ast);
        std::cout << "Semantic analysis passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
