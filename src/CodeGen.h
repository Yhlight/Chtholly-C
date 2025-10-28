#ifndef CHTHOLLY_CODEGEN_H
#define CHTHOLLY_CODEGEN_H

#include "AST.h"
#include "llvm/IR/IRBuilder.h"
#include <memory>
#include <string>

namespace Chtholly {

class CodeGen {
public:
    CodeGen();
    void generate(Program& program, std::string& out);

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    llvm::Value* generate(ASTNode* node);
    llvm::Value* generate(IntegerLiteral* node);
    llvm::Value* generate(VariableDeclaration* node);
    llvm::Value* generate(BinaryExpression* node);
};

} // namespace Chtholly

#endif // CHTHOLLY_CODEGEN_H
