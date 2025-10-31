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

class AssignExprAST : public ExprAST {
    std::unique_ptr<ExprAST> target;
    std::unique_ptr<ExprAST> value;
public:
    AssignExprAST(std::unique_ptr<ExprAST> target, std::unique_ptr<ExprAST> value)
        : target(std::move(target)), value(std::move(value)) {}

    const ExprAST& getTarget() const { return *target; }
    const ExprAST& getValue() const { return *value; }
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

class WhileStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> cond;
    std::unique_ptr<BlockStmtAST> body;
public:
    WhileStmtAST(std::unique_ptr<ExprAST> cond, std::unique_ptr<BlockStmtAST> body)
        : cond(std::move(cond)), body(std::move(body)) {}

    const ExprAST& getCond() const { return *cond; }
    const BlockStmtAST& getBody() const { return *body; }
};

class ForStmtAST : public StmtAST {
    std::unique_ptr<StmtAST> init; // Can be nullptr or VarDecl
    std::unique_ptr<ExprAST> cond; // Can be nullptr
    std::unique_ptr<ExprAST> inc;  // Can be nullptr
    std::unique_ptr<BlockStmtAST> body;
public:
    ForStmtAST(std::unique_ptr<StmtAST> init, std::unique_ptr<ExprAST> cond, std::unique_ptr<ExprAST> inc, std::unique_ptr<BlockStmtAST> body)
        : init(std::move(init)), cond(std::move(cond)), inc(std::move(inc)), body(std::move(body)) {}

    const StmtAST* getInit() const { return init.get(); }
    const ExprAST* getCond() const { return cond.get(); }
    const ExprAST* getInc() const { return inc.get(); }
    const BlockStmtAST& getBody() const { return *body; }
};

class ImportStmtAST : public StmtAST {
    std::string moduleName;
public:
    ImportStmtAST(std::string moduleName) : moduleName(std::move(moduleName)) {}
    const std::string& getModuleName() const { return moduleName; }
};

class ExprStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> expr;
public:
    ExprStmtAST(std::unique_ptr<ExprAST> expr) : expr(std::move(expr)) {}
    const ExprAST& getExpr() const { return *expr; }
};

struct MemberVar {
    std::string name;
    std::string typeName;
    bool isPublic;
};

class StructDeclAST : public StmtAST {
    std::string name;
    std::vector<MemberVar> members;
public:
    StructDeclAST(std::string name, std::vector<MemberVar> members)
        : name(std::move(name)), members(std::move(members)) {}

    const std::string& getName() const { return name; }
    const std::vector<MemberVar>& getMembers() const { return members; }
};

struct MemberInit {
    std::string name;
    std::unique_ptr<ExprAST> value;
};

class StructInitExprAST : public ExprAST {
    std::string structName;
    std::vector<MemberInit> members;
public:
    StructInitExprAST(std::string structName, std::vector<MemberInit> members)
        : structName(std::move(structName)), members(std::move(members)) {}

    const std::string& getStructName() const { return structName; }
    const std::vector<MemberInit>& getMembers() const { return members; }
};

class MemberAccessExprAST : public ExprAST {
    std::unique_ptr<ExprAST> object;
    std::string memberName;
public:
    MemberAccessExprAST(std::unique_ptr<ExprAST> object, std::string memberName)
        : object(std::move(object)), memberName(std::move(memberName)) {}

    const ExprAST& getObject() const { return *object; }
    const std::string& getMemberName() const { return memberName; }
};

class TraitDeclAST : public StmtAST {
    std::string name;
    std::vector<std::unique_ptr<FuncDeclAST>> methods;
public:
    TraitDeclAST(std::string name, std::vector<std::unique_ptr<FuncDeclAST>> methods)
        : name(std::move(name)), methods(std::move(methods)) {}

    const std::string& getName() const { return name; }
    const std::vector<std::unique_ptr<FuncDeclAST>>& getMethods() const { return methods; }
};

} // namespace chtholly
