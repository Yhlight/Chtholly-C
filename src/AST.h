#pragma once

#include "Token.h"
#include <memory>
#include <any>
#include <vector>
#include <string>
#include <stdexcept>

namespace chtholly {

// Forward declarations for Expr
struct LiteralExpr;
struct UnaryExpr;
struct BinaryExpr;
struct VariableExpr;
struct GroupingExpr;

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
};

// Base class for all expression nodes
class Expr {
public:
    virtual ~Expr() = default;
    template <typename R>
    R accept(ExprVisitor<R>& visitor) const;
};

// Expression nodes
struct LiteralExpr : Expr {
    LiteralExpr(Token token) : token(std::move(token)) {}
    const Token token;
};

struct UnaryExpr : Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}
    const Token op;
    const std::unique_ptr<Expr> right;
};

struct BinaryExpr : Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};

struct VariableExpr : Expr {
    VariableExpr(Token name) : name(std::move(name)) {}
    const Token name;
};

struct GroupingExpr : Expr {
    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    const std::unique_ptr<Expr> expression;
};

// Implementation of the generic accept method for Expr
template <typename R>
R Expr::accept(ExprVisitor<R>& visitor) const {
    if (auto p = dynamic_cast<const LiteralExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const UnaryExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const BinaryExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const VariableExpr*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const GroupingExpr*>(this)) return visitor.visit(*p);
    throw std::runtime_error("Unknown expression type in accept.");
}


// Forward declarations for Stmt
struct VarDeclStmt;
struct ExprStmt;
struct BlockStmt;
struct IfStmt;
struct ForStmt;

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
};

// Base class for all statement nodes
class Stmt {
public:
    virtual ~Stmt() = default;
    template <typename R>
    R accept(StmtVisitor<R>& visitor) const;
};

// Statement nodes
struct VarDeclStmt : Stmt {
    VarDeclStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}
    const Token name;
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

// Implementation of the generic accept method for Stmt
template <typename R>
R Stmt::accept(StmtVisitor<R>& visitor) const {
    if (auto p = dynamic_cast<const VarDeclStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const ExprStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const BlockStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const IfStmt*>(this)) return visitor.visit(*p);
    if (auto p = dynamic_cast<const ForStmt*>(this)) return visitor.visit(*p);
    throw std::runtime_error("Unknown statement type in accept.");
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

    std::string visit(const VarDeclStmt& stmt) override { return "(var " + stmt.name.lexeme + " = " + print(*stmt.initializer) + ")"; }
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
        std::string result = "(for " + print(*stmt.initializer) + " " + print(*stmt.condition) + " " + print(*stmt.increment) + " " + print(*stmt.body) + ")";
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
