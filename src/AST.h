#pragma once

#include "Token.h"
#include <memory>
#include <vector>
#include <variant>

namespace chtholly {

class Sema; // Forward declaration
class CodeGen; // Forward declaration

class ExprAST {
public:
    virtual ~ExprAST() = default;
    friend class Sema;
    friend class CodeGen;
};

class NumberExprAST : public ExprAST {
    std::variant<int, double> val;
public:
    NumberExprAST(std::variant<int, double> val) : val(val) {}
    const std::variant<int, double>& getVal() const { return val; }
};

class StringExprAST : public ExprAST {
    std::string val;
public:
    StringExprAST(std::string val) : val(std::move(val)) {}
    const std::string& getVal() const { return val; }
};

class VariableExprAST : public ExprAST {
    std::string name;
public:
    VariableExprAST(std::string name) : name(std::move(name)) {}
    const std::string& getName() const { return name; }
};

class BinaryExprAST : public ExprAST {
    TokenType op;
    std::unique_ptr<ExprAST> lhs, rhs;
public:
    BinaryExprAST(TokenType op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    const ExprAST& getLHS() const { return *lhs; }
    const ExprAST& getRHS() const { return *rhs; }
    TokenType getOp() const { return op; }
};

class StmtAST : public ExprAST {
    friend class Sema;
    friend class CodeGen;
};

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
