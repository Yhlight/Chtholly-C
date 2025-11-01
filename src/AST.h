#pragma once

#include "Token.h"
#include <memory>
#include <any>
#include <vector>
#include <optional>
#include <string>
#include <stdexcept>

namespace chtholly {

// Forward declarations for Expr
struct LiteralExpr;
struct UnaryExpr;
struct BinaryExpr;
struct VariableExpr;
struct GroupingExpr;
struct CallExpr;
struct LambdaExpr;
struct StructInitExpr;
struct MemberAccessExpr;

// Forward declarations for Stmt
struct VarDeclStmt;
struct ExprStmt;
struct BlockStmt;
struct IfStmt;
struct ForStmt;
struct FuncDeclStmt;
struct ReturnStmt;
struct SwitchStmt;
struct CaseStmt;
struct BreakStmt;
struct FallthroughStmt;
struct StructDeclStmt;


// Expr Visitor interface
template <typename R>
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual R visit(const LiteralExpr& expr) = 0;
    virtual R visit(const UnaryExpr& expr) = 0;
    virtual R visit(const BinaryExpr& expr) = 0;
    virtual R visit(const VariableExpr& expr) = 0;
    virtual R visit(const GroupingExpr& expr) = 0;
    virtual R visit(const CallExpr& expr) = 0;
    virtual R visit(const LambdaExpr& expr) = 0;
    virtual R visit(const StructInitExpr& expr) = 0;
    virtual R visit(const MemberAccessExpr& expr) = 0;
};

// Base class for all expression nodes
class Expr {
public:
    virtual ~Expr() = default;
    template <typename R>
    R accept(ExprVisitor<R>& visitor) const;
    virtual const Token& getToken() const = 0;
};

// Expression nodes
struct LiteralExpr : Expr {
    LiteralExpr(Token token) : token(std::move(token)) {}
    const Token token;
    const Token& getToken() const override { return token; }
};

struct UnaryExpr : Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}
    const Token op;
    const std::unique_ptr<Expr> right;
    const Token& getToken() const override { return op; }
};

struct BinaryExpr : Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
    const Token& getToken() const override { return op; }
};

struct VariableExpr : Expr {
    VariableExpr(Token name) : name(std::move(name)) {}
    const Token name;
    const Token& getToken() const override { return name; }
};

struct GroupingExpr : Expr {
    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    const std::unique_ptr<Expr> expression;
    const Token& getToken() const override { return expression->getToken(); }
};

struct CallExpr : Expr {
    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    const std::unique_ptr<Expr> callee;
    const Token paren; // The closing ')' for error reporting.
    const std::vector<std::unique_ptr<Expr>> arguments;
    const Token& getToken() const override { return paren; }
};

struct StructInitExpr : Expr {
    struct MemberInit {
        Token name;
        std::unique_ptr<Expr> initializer;
    };

    StructInitExpr(Token name, std::vector<MemberInit> members)
        : name(std::move(name)), members(std::move(members)) {}

    const Token name;
    const std::vector<MemberInit> members;
    const Token& getToken() const override { return name; }
};

struct MemberAccessExpr : Expr {
    MemberAccessExpr(std::unique_ptr<Expr> object, Token name)
        : object(std::move(object)), name(std::move(name)) {}

    const std::unique_ptr<Expr> object;
    const Token name;
    const Token& getToken() const override { return name; }
};


// Stmt Visitor interface
template <typename R>
class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual R visit(const VarDeclStmt& stmt) = 0;
    virtual R visit(const ExprStmt& stmt) = 0;
    virtual R visit(const BlockStmt& stmt) = 0;
    virtual R visit(const IfStmt& stmt) = 0;
    virtual R visit(const ForStmt& stmt) = 0;
    virtual R visit(const FuncDeclStmt& stmt) = 0;
    virtual R visit(const ReturnStmt& stmt) = 0;
    virtual R visit(const SwitchStmt& stmt) = 0;
    virtual R visit(const CaseStmt& stmt) = 0;
    virtual R visit(const BreakStmt& stmt) = 0;
    virtual R visit(const FallthroughStmt& stmt) = 0;
    virtual R visit(const StructDeclStmt& stmt) = 0;
};

// Base class for all statement nodes
class Stmt {
public:
    virtual ~Stmt() = default;
    template <typename R>
    R accept(StmtVisitor<R>& visitor) const;
    virtual bool isFallthrough() const { return false; }
};

// Statement nodes
struct VarDeclStmt : Stmt {
    VarDeclStmt(Token keyword, Token name, std::optional<Token> type, std::unique_ptr<Expr> initializer)
        : keyword(std::move(keyword)), name(std::move(name)), type(std::move(type)), initializer(std::move(initializer)) {}
    const Token keyword; // let or mut
    const Token name;
    const std::optional<Token> type;
    const std::unique_ptr<Expr> initializer;
};

struct ExprStmt : Stmt {
    ExprStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    const std::unique_ptr<Expr> expression;
};

struct BlockStmt : Stmt {
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}
    const std::vector<std::unique_ptr<Stmt>> statements;
};

struct IfStmt : Stmt {
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Stmt> thenBranch;
    const std::unique_ptr<Stmt> elseBranch;
};

struct ForStmt : Stmt {
    ForStmt(std::unique_ptr<Stmt> initializer, std::unique_ptr<Expr> condition,
            std::unique_ptr<Expr> increment, std::unique_ptr<Stmt> body)
        : initializer(std::move(initializer)), condition(std::move(condition)),
          increment(std::move(increment)), body(std::move(body)) {}
    const std::unique_ptr<Stmt> initializer;
    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Expr> increment;
    const std::unique_ptr<Stmt> body;
};

struct FuncDeclStmt : Stmt {
    struct Param {
        Token name;
        Token type;
    };

    FuncDeclStmt(Token name, std::vector<Param> params, std::optional<Token> returnType, std::unique_ptr<BlockStmt> body)
        : name(std::move(name)), params(std::move(params)), returnType(std::move(returnType)), body(std::move(body)) {}

    const Token name;
    const std::vector<Param> params;
    const std::optional<Token> returnType;
    const std::unique_ptr<BlockStmt> body;
};

struct ReturnStmt : Stmt {
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}

    const Token keyword;
    const std::unique_ptr<Expr> value;
};

struct BreakStmt : Stmt {
    BreakStmt(Token keyword) : keyword(std::move(keyword)) {}
    const Token keyword;
};

struct FallthroughStmt : Stmt {
    FallthroughStmt(Token keyword) : keyword(std::move(keyword)) {}
    const Token keyword;
    bool isFallthrough() const override { return true; }
};

struct CaseStmt : Stmt {
    CaseStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}
    const std::unique_ptr<Expr> condition; // nullptr for default case
    const std::unique_ptr<Stmt> body;
};

struct SwitchStmt : Stmt {
    SwitchStmt(std::unique_ptr<Expr> condition, std::vector<std::unique_ptr<CaseStmt>> cases)
        : condition(std::move(condition)), cases(std::move(cases)) {}
    const std::unique_ptr<Expr> condition;
    const std::vector<std::unique_ptr<CaseStmt>> cases;
};

struct StructDeclStmt : Stmt {
    enum class Visibility { PUBLIC, PRIVATE };

    struct MemberVar {
        Visibility visibility;
        Token name;
        Token type;
    };

    StructDeclStmt(Token name, std::vector<MemberVar> members, std::vector<std::unique_ptr<FuncDeclStmt>> methods)
        : name(std::move(name)), members(std::move(members)), methods(std::move(methods)) {}

    const Token name;
    const std::vector<MemberVar> members;
    const std::vector<std::unique_ptr<FuncDeclStmt>> methods;
};

// Now define LambdaExpr, which depends on FuncDeclStmt::Param
struct LambdaExpr : Expr {
    LambdaExpr(Token keyword, std::vector<FuncDeclStmt::Param> params, std::optional<Token> returnType, std::unique_ptr<BlockStmt> body)
        : keyword(std::move(keyword)), params(std::move(params)), returnType(std::move(returnType)), body(std::move(body)) {}

    const Token keyword; // The opening '['
    const std::vector<FuncDeclStmt::Param> params;
    const std::optional<Token> returnType;
    const std::unique_ptr<BlockStmt> body;
    const Token& getToken() const override { return keyword; }
};


// Implementation of the generic accept method for Stmt
template <typename R>
R Stmt::accept(StmtVisitor<R>& visitor) const {
    if (auto p = dynamic_cast<const VarDeclStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const ExprStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const BlockStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const IfStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const ForStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const FuncDeclStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const ReturnStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const SwitchStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const CaseStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const BreakStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const FallthroughStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const StructDeclStmt*>(this)) return visitor.visit(*p);
    throw std::runtime_error("Unknown statement type in accept.");
}

// Implementation of the generic accept method for Expr
template <typename R>
R Expr::accept(ExprVisitor<R>& visitor) const {
    if (auto p = dynamic_cast<const LiteralExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const UnaryExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const BinaryExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const VariableExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const GroupingExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const CallExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const LambdaExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const StructInitExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const MemberAccessExpr*>(this)) return visitor.visit(*p);
    throw std::runtime_error("Unknown expression type in accept.");
}


// A custom exception for parse errors
struct ParseError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

// A simple AST printer for testing
class AstPrinter : public ExprVisitor<std::string>, public StmtVisitor<std::string> {
public:
    std::string print(const Stmt& stmt) { return stmt.accept(*this); }
    std::string print(const Expr& expr) { return expr.accept(*this); }

    std::string visit(const LiteralExpr& expr) override { return expr.token.lexeme; }
    std::string visit(const UnaryExpr& expr) override { return parenthesize(expr.op.lexeme, {expr.right.get()}); }
    std::string visit(const BinaryExpr& expr) override { return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()}); }
    std::string visit(const VariableExpr& expr) override { return expr.name.lexeme; }
    std::string visit(const GroupingExpr& expr) override { return parenthesize("group", {expr.expression.get()}); }
    std::string visit(const CallExpr& expr) override {
        std::vector<const Expr*> exprs;
        exprs.push_back(expr.callee.get());
        for(const auto& arg : expr.arguments) {
            exprs.push_back(arg.get());
        }
        return parenthesize("call", exprs);
    }
    std::string visit(const LambdaExpr& expr) override {
        std::string result = "(lambda [](";
        for (size_t i = 0; i < expr.params.size(); ++i) {
            result += expr.params[i].name.lexeme + ": " + expr.params[i].type.lexeme;
            if (i < expr.params.size() - 1) {
                result += ", ";
            }
        }
        result += ")";
        if (expr.returnType) {
            result += " -> " + expr.returnType->lexeme;
        }
        result += " " + print(*expr.body) + ")";
        return result;
    }

    std::string visit(const StructInitExpr& expr) override {
        std::string result = "(init " + expr.name.lexeme;
        for (const auto& member : expr.members) {
            result += " (member " + member.name.lexeme + " = " + print(*member.initializer) + ")";
        }
        result += ")";
        return result;
    }

    std::string visit(const MemberAccessExpr& expr) override {
        return "(. " + print(*expr.object) + " " + expr.name.lexeme + ")";
    }

    std::string visit(const VarDeclStmt& stmt) override {
        std::string result = "(var " + stmt.name.lexeme;
        if (stmt.type) {
            result += ": " + stmt.type->lexeme;
        }
        if (stmt.initializer) {
            result += " = " + print(*stmt.initializer);
        }
        result += ")";
        return result;
    }
    std::string visit(const ExprStmt& stmt) override { return "(; " + print(*stmt.expression) + ")"; }

    std::string visit(const BlockStmt& stmt) override {
        std::string result = "(block";
        for (const auto& statement : stmt.statements) {
            result += " " + print(*statement);
        }
        result += ")";
        return result;
    }

    std::string visit(const IfStmt& stmt) override {
        std::string result = "(if " + print(*stmt.condition) + " " + print(*stmt.thenBranch);
        if (stmt.elseBranch) {
            result += " " + print(*stmt.elseBranch);
        }
        result += ")";
        return result;
    }

    std::string visit(const ForStmt& stmt) override {
        std::string result = "(for ";
        if(stmt.initializer) result += print(*stmt.initializer);
        result += " ";
        if(stmt.condition) result += print(*stmt.condition);
        result += " ";
        if(stmt.increment) result += print(*stmt.increment);
        result += " " + print(*stmt.body) + ")";
        return result;
    }

    std::string visit(const FuncDeclStmt& stmt) override {
        std::string result = "(func " + stmt.name.lexeme + "(";
        for (size_t i = 0; i < stmt.params.size(); ++i) {
            result += stmt.params[i].name.lexeme + ": " + stmt.params[i].type.lexeme;
            if (i < stmt.params.size() - 1) {
                result += ", ";
            }
        }
        result += ")";
        if (stmt.returnType) {
            result += " -> " + stmt.returnType->lexeme;
        }
        result += " " + print(*stmt.body) + ")";
        return result;
    }

    std::string visit(const ReturnStmt& stmt) override {
        return "(return " + (stmt.value ? print(*stmt.value) : "") + ")";
    }

    std::string visit(const SwitchStmt& stmt) override {
        std::string result = "(switch " + print(*stmt.condition);
        for (const auto& caseStmt : stmt.cases) {
            result += " " + print(*caseStmt);
        }
        result += ")";
        return result;
    }

    std::string visit(const CaseStmt& stmt) override {
        std::string result = "(case";
        if (stmt.condition) {
            result += " " + print(*stmt.condition);
        } else {
            result += " default";
        }
        result += " " + print(*stmt.body) + ")";
        return result;
    }

    std::string visit(const BreakStmt& stmt) override {
        return "(break)";
    }

    std::string visit(const FallthroughStmt& stmt) override {
        return "(fallthrough)";
    }

    std::string visit(const StructDeclStmt& stmt) override {
        std::string result = "(struct " + stmt.name.lexeme;
        for (const auto& member : stmt.members) {
            result += " (member ";
            result += (member.visibility == StructDeclStmt::Visibility::PUBLIC) ? "public " : "private ";
            result += member.name.lexeme + ": " + member.type.lexeme + ")";
        }
        for (const auto& method : stmt.methods) {
            result += " " + print(*method);
        }
        result += ")";
        return result;
    }

private:
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
        std::string result = "(" + name;
        for (const auto& expr : exprs) {
            result += " ";
            result += expr->accept(*this);
        }
        result += ")";
        return result;
    }
};

} // namespace chtholly
