#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

int main() {
    llvm::LLVMContext context;
    llvm::Module module("Chtholly", context);

    std::cout << "Hello, Chtholly!" << std::endl;
    std::cout << "Successfully initialized LLVM." << std::endl;

    return 0;
}
