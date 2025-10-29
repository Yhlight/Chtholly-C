#ifndef CHTHOLLY_CODEGEN_H
#define CHTHOLLY_CODEGEN_H

#include "AST.h"
#include "llvm/IR/IRBuilder.h"
#include <memory>
#include <string>
#include <map>

namespace Chtholly {

class CodeGen {
public:
    CodeGen();
    void generate(Program& program, std::string& out);

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::map<std::string, llvm::AllocaInst*> namedValues;

    llvm::Value* generate(ASTNode* node);
    llvm::Value* generate(Identifier* node);
    llvm::Value* generate(IntegerLiteral* node);
    llvm::Value* generate(DoubleLiteral* node);
    llvm::Value* generate(StringLiteral* node);
    llvm::Value* generate(VariableDeclaration* node);
    llvm::Value* generate(BinaryExpression* node);
    llvm::Value* generate(ExpressionStatement* node);
    llvm::Value* generate(ReturnStatement* node);
    llvm::Value* generate(BlockStatement* node);
    llvm::Value* generate(FunctionDeclaration* node);
    llvm::Value* generate(FunctionCall* node);
    llvm::Value* generate(IfExpression* node);
    llvm::Value* generate(WhileStatement* node);
};

} // namespace Chtholly

#endif // CHTHOLLY_CODEGEN_H
