#include "CodeGen.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

namespace Chtholly {

CodeGen::CodeGen() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("Chtholly", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

void CodeGen::generate(Program& program, std::string& out) {
    // Create a main function
    llvm::FunctionType* funcType = llvm::FunctionType::get(builder->getInt32Ty(), false);
    llvm::Function* mainFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module.get());
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", mainFunc);
    builder->SetInsertPoint(entry);

    for (auto& stmt : program.statements) {
        generate(stmt.get());
    }

    // Add a return statement to the main function
    builder->CreateRet(builder->getInt32(0));

    llvm::verifyFunction(*mainFunc);

    llvm::raw_string_ostream os(out);
    module->print(os, nullptr);
}

llvm::Value* CodeGen::generate(ASTNode* node) {
    if (auto* intLit = dynamic_cast<IntegerLiteral*>(node)) {
        return generate(intLit);
    }
    if (auto* varDecl = dynamic_cast<VariableDeclaration*>(node)) {
        return generate(varDecl);
    }
    if (auto* binExpr = dynamic_cast<BinaryExpression*>(node)) {
        return generate(binExpr);
    }
    // Add other node types here...
    return nullptr;
}

llvm::Value* CodeGen::generate(IntegerLiteral* node) {
    return llvm::ConstantInt::get(*context, llvm::APInt(64, node->value, true));
}

llvm::Value* CodeGen::generate(VariableDeclaration* node) {
    llvm::Value* value = generate(node->value.get());
    if (!value) return nullptr;

    // Create an alloca for the variable
    llvm::AllocaInst* alloca = builder->CreateAlloca(value->getType(), nullptr, node->name->token.literal);

    // Store the initial value
    builder->CreateStore(value, alloca);

    return alloca;
}

llvm::Value* CodeGen::generate(BinaryExpression* node) {
    llvm::Value* left = generate(node->left.get());
    llvm::Value* right = generate(node->right.get());
    if (!left || !right) return nullptr;

    switch (node->op.type) {
        case TokenType::Plus:
            return builder->CreateAdd(left, right, "addtmp");
        case TokenType::Minus:
            return builder->CreateSub(left, right, "subtmp");
        case TokenType::Asterisk:
            return builder->CreateMul(left, right, "multmp");
        case TokenType::Slash:
            return builder->CreateSDiv(left, right, "divtmp");
        default:
            return nullptr;
    }
}

} // namespace Chtholly
