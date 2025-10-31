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

class CallExprAST : public ExprAST {
    std::unique_ptr<ExprAST> callee;
    std::vector<std::unique_ptr<ExprAST>> args;
public:
    CallExprAST(std::unique_ptr<ExprAST> callee, std::vector<std::unique_ptr<ExprAST>> args)
        : callee(std::move(callee)), args(std::move(args)) {}

    const ExprAST& getCallee() const { return *callee; }
    const std::vector<std::unique_ptr<ExprAST>>& getArgs() const { return args; }
};

class BoolExprAST : public ExprAST {
    bool val;
public:
    BoolExprAST(bool val) : val(val) {}
    bool getVal() const { return val; }
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

struct Param {
    std::string name;
    // TODO: Add full type information
    std::string typeName; // Placeholder for now
};

class FuncDeclAST : public StmtAST {
    std::string name;
    std::vector<Param> params;
    // TODO: Add return type information
    std::string returnTypeName; // Placeholder for now
    std::unique_ptr<BlockStmtAST> body;
public:
    FuncDeclAST(std::string name, std::vector<Param> params, std::string returnTypeName, std::unique_ptr<BlockStmtAST> body)
        : name(std::move(name)), params(std::move(params)), returnTypeName(std::move(returnTypeName)), body(std::move(body)) {}

    const std::string& getName() const { return name; }
    const std::vector<Param>& getParams() const { return params; }
    const std::string& getReturnTypeName() const { return returnTypeName; }
    const BlockStmtAST& getBody() const { return *body; }
};

class IfStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<BlockStmtAST> thenBranch;
    std::unique_ptr<BlockStmtAST> elseBranch; // Can be nullptr
public:
    IfStmtAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<BlockStmtAST> thenBranch, std::unique_ptr<BlockStmtAST> elseBranch)
        : cond(std::move(cond)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    const ExprAST& getCond() const { return *cond; }
    const BlockStmtAST& getThen() const { return *thenBranch; }
    const BlockStmtAST* getElse() const { return elseBranch.get(); }
};

class CaseBlockAST : public StmtAST {
    std::unique_ptr<ExprAST> expr; // Can be nullptr for default case
    std::unique_ptr<BlockStmtAST> body;
public:
    CaseBlockAST(std::unique_ptr<ExprAST> expr, std::unique_ptr<BlockStmtAST> body)
        : expr(std::move(expr)), body(std::move(body)) {}

    const ExprAST* getExpr() const { return expr.get(); }
    const BlockStmtAST& getBody() const { return *body; }
};

class SwitchStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> expr;
    std::vector<std::unique_ptr<CaseBlockAST>> cases;
public:
    SwitchStmtAST(std::unique_ptr<ExprAST> expr, std::vector<std::unique_ptr<CaseBlockAST>> cases)
        : expr(std::move(expr)), cases(std::move(cases)) {}

    const ExprAST& getExpr() const { return *expr; }
    const std::vector<std::unique_ptr<CaseBlockAST>>& getCases() const { return cases; }
};

class EnumDeclAST : public StmtAST {
    std::string name;
    std::vector<std::string> members;
public:
    EnumDeclAST(std::string name, std::vector<std::string> members)
        : name(std::move(name)), members(std::move(members)) {}

    const std::string& getName() const { return name; }
    const std::vector<std::string>& getMembers() const { return members; }
};

} // namespace chtholly
