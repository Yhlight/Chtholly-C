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

    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateRet(builder->getInt32(0));
    }

    llvm::verifyFunction(*mainFunc);

    llvm::raw_string_ostream os(out);
    module->print(os, nullptr);
}

llvm::Value* CodeGen::generate(ASTNode* node) {
    if (auto* ident = dynamic_cast<Identifier*>(node)) {
        return generate(ident);
    }
    if (auto* intLit = dynamic_cast<IntegerLiteral*>(node)) {
        return generate(intLit);
    }
    if (auto* doubleLit = dynamic_cast<DoubleLiteral*>(node)) {
        return generate(doubleLit);
    }
    if (auto* stringLit = dynamic_cast<StringLiteral*>(node)) {
        return generate(stringLit);
    }
    if (auto* varDecl = dynamic_cast<VariableDeclaration*>(node)) {
        return generate(varDecl);
    }
    if (auto* binExpr = dynamic_cast<BinaryExpression*>(node)) {
        return generate(binExpr);
    }
    if (auto* exprStmt = dynamic_cast<ExpressionStatement*>(node)) {
        return generate(exprStmt);
    }
    if (auto* retStmt = dynamic_cast<ReturnStatement*>(node)) {
        return generate(retStmt);
    }
    if (auto* blockStmt = dynamic_cast<BlockStatement*>(node)) {
        return generate(blockStmt);
    }
    if (auto* funcDecl = dynamic_cast<FunctionDeclaration*>(node)) {
        return generate(funcDecl);
    }
    if (auto* funcCall = dynamic_cast<FunctionCall*>(node)) {
        return generate(funcCall);
    }
    if (auto* ifExpr = dynamic_cast<IfExpression*>(node)) {
        return generate(ifExpr);
    }
    if (auto* whileStmt = dynamic_cast<WhileStatement*>(node)) {
        return generate(whileStmt);
    }
    // Add other node types here...
    return nullptr;
}

llvm::Value* CodeGen::generate(WhileStatement* node) {
    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* loopCondBB = llvm::BasicBlock::Create(*context, "loopcond", function);
    llvm::BasicBlock* loopBodyBB = llvm::BasicBlock::Create(*context, "loopbody", function);
    llvm::BasicBlock* loopExitBB = llvm::BasicBlock::Create(*context, "loopexit", function);

    builder->CreateBr(loopCondBB);

    builder->SetInsertPoint(loopCondBB);
    llvm::Value* condV = generate(node->condition.get());
    condV = builder->CreateICmpNE(condV, llvm::ConstantInt::get(*context, llvm::APInt(1, 0)), "loopcond");
    builder->CreateCondBr(condV, loopBodyBB, loopExitBB);

    builder->SetInsertPoint(loopBodyBB);
    generate(node->body.get());
    builder->CreateBr(loopCondBB);

    builder->SetInsertPoint(loopExitBB);

    return nullptr;
}

llvm::Value* CodeGen::generate(IntegerLiteral* node) {
    return llvm::ConstantInt::get(*context, llvm::APInt(64, node->value, true));
}

llvm::Value* CodeGen::generate(DoubleLiteral* node) {
    return llvm::ConstantFP::get(*context, llvm::APFloat(node->value));
}

llvm::Value* CodeGen::generate(StringLiteral* node) {
    return builder->CreateGlobalStringPtr(node->token.literal);
}

llvm::Value* CodeGen::generate(Identifier* node) {
    llvm::AllocaInst* alloca = namedValues[node->token.literal];
    if (!alloca) {
        // Handle error: unknown variable
        return nullptr;
    }
    return builder->CreateLoad(alloca->getAllocatedType(), alloca, node->token.literal);
}

llvm::Value* CodeGen::generate(VariableDeclaration* node) {
    llvm::Value* value = generate(node->value.get());
    if (!value) return nullptr;

    // Create an alloca for the variable
    llvm::AllocaInst* alloca = builder->CreateAlloca(value->getType(), nullptr, node->name->token.literal);

    // Store the initial value
    builder->CreateStore(value, alloca);

    // Add to named values map
    namedValues[node->name->token.literal] = alloca;

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

llvm::Value* CodeGen::generate(ExpressionStatement* node) {
    return generate(node->expression.get());
}

llvm::Value* CodeGen::generate(ReturnStatement* node) {
    llvm::Value* returnValue = generate(node->returnValue.get());
    return builder->CreateRet(returnValue);
}

llvm::Value* CodeGen::generate(BlockStatement* node) {
    for (auto& stmt : node->statements) {
        generate(stmt.get());
    }
    return nullptr;
}

llvm::Value* CodeGen::generate(FunctionDeclaration* node) {
    std::vector<llvm::Type*> paramTypes;
    for (auto& param : node->parameters) {
        paramTypes.push_back(builder->getInt64Ty()); // Assuming all params are int64 for now
    }
    llvm::FunctionType* funcType = llvm::FunctionType::get(builder->getInt64Ty(), paramTypes, false);
    llvm::Function* function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node->name->token.literal, module.get());

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(entry);

    auto oldNamedValues = namedValues;
    namedValues.clear();
    for (auto& arg : function->args()) {
        llvm::AllocaInst* alloca = builder->CreateAlloca(arg.getType(), nullptr, arg.getName());
        builder->CreateStore(&arg, alloca);
        namedValues[std::string(arg.getName())] = alloca;
    }

    generate(node->body.get());

    llvm::verifyFunction(*function);
    namedValues = oldNamedValues;
    return function;
}

llvm::Value* CodeGen::generate(FunctionCall* node) {
    llvm::Function* calleeFunc = module->getFunction(dynamic_cast<Identifier*>(node->callee.get())->token.literal);
    if (!calleeFunc) {
        // Handle error: function not found
        return nullptr;
    }

    if (calleeFunc->arg_size() != node->arguments.size()) {
        // Handle error: incorrect number of arguments
        return nullptr;
    }

    std::vector<llvm::Value*> argsV;
    for (auto& arg : node->arguments) {
        argsV.push_back(generate(arg.get()));
        if (!argsV.back()) {
            return nullptr;
        }
    }

    return builder->CreateCall(calleeFunc, argsV, "calltmp");
}

llvm::Value* CodeGen::generate(IfExpression* node) {
    llvm::Value* condV = generate(node->condition.get());
    if (!condV) return nullptr;

    condV = builder->CreateICmpNE(condV, llvm::ConstantInt::get(*context, llvm::APInt(1, 0)), "ifcond");

    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", function);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*context, "else", function);
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont", function);

    builder->CreateCondBr(condV, thenBB, elseBB);

    builder->SetInsertPoint(thenBB);
    generate(node->consequence.get());
    builder->CreateBr(mergeBB);

    builder->SetInsertPoint(elseBB);
    if (node->alternative) {
        generate(node->alternative.get());
    }
    builder->CreateBr(mergeBB);

    builder->SetInsertPoint(mergeBB);

    return nullptr;
}

} // namespace Chtholly
