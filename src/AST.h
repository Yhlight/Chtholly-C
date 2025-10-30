#pragma once

#include "Token.h"
#include <memory>
#include <vector>

namespace chtholly {

class ExprAST {
public:
    virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST {
    std::variant<int, double> val;
public:
    NumberExprAST(std::variant<int, double> val) : val(val) {}
};

class VariableExprAST : public ExprAST {
    std::string name;
public:
    VariableExprAST(std::string name) : name(std::move(name)) {}
};

class BinaryExprAST : public ExprAST {
    TokenType op;
    std::unique_ptr<ExprAST> lhs, rhs;
public:
    BinaryExprAST(TokenType op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

class StmtAST : public ExprAST {};

class VarDeclAST : public StmtAST {
    std::string name;
    // TODO: Add type information
    std::unique_ptr<ExprAST> init;
    bool isMutable;
public:
    VarDeclAST(std::string name, std::unique_ptr<ExprAST> init, bool isMutable)
        : name(std::move(name)), init(std::move(init)), isMutable(isMutable) {}

    const std::string& getName() const { return name; }
    bool getIsMutable() const { return isMutable; }
    const ExprAST* getInit() const { return init.get(); }
};

class BlockStmtAST : public StmtAST {
    std::vector<std::unique_ptr<StmtAST>> statements;
public:
    BlockStmtAST(std::vector<std::unique_ptr<StmtAST>> statements)
        : statements(std::move(statements)) {}

    const std::vector<std::unique_ptr<StmtAST>>& getStatements() const {
        return statements;
    }
};

} // namespace chtholly
