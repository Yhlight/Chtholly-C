#pragma once

#include "Token.h"
#include <any>
#include <memory>
#include <vector>

namespace chtholly {

// Forward declarations for Expressions
struct AssignExpr;
struct BinaryExpr;
struct CallExpr;
struct GroupingExpr;
struct LiteralExpr;
struct LogicalExpr;
struct UnaryExpr;
struct VariableExpr;

// Visitor for Expressions
class ExprVisitor {
public:
    virtual std::any visitAssignExpr(std::shared_ptr<AssignExpr> expr) = 0;
    virtual std::any visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) = 0;
    virtual std::any visitCallExpr(std::shared_ptr<CallExpr> expr) = 0;
    virtual std::any visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) = 0;
    virtual std::any visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) = 0;
    virtual std::any visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) = 0;
    virtual std::any visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) = 0;
    virtual std::any visitVariableExpr(std::shared_ptr<VariableExpr> expr) = 0;
    virtual ~ExprVisitor() = default;
};

// Base class for all expressions
struct Expr {
    virtual std::any accept(ExprVisitor& visitor) = 0;
};

// Expression nodes
struct AssignExpr : Expr, public std::enable_shared_from_this<AssignExpr> {
    AssignExpr(Token name, std::shared_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitAssignExpr(shared_from_this());
    }

    const Token name;
    const std::shared_ptr<Expr> value;
};

struct BinaryExpr : Expr, public std::enable_shared_from_this<BinaryExpr> {
    BinaryExpr(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitBinaryExpr(shared_from_this());
    }

    const std::shared_ptr<Expr> left;
    const Token op;
    const std::shared_ptr<Expr> right;
};

struct CallExpr : Expr, public std::enable_shared_from_this<CallExpr> {
    CallExpr(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitCallExpr(shared_from_this());
    }

    const std::shared_ptr<Expr> callee;
    const Token paren; // for error reporting
    const std::vector<std::shared_ptr<Expr>> arguments;
};

struct GroupingExpr : Expr, public std::enable_shared_from_this<GroupingExpr> {
    GroupingExpr(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitGroupingExpr(shared_from_this());
    }

    const std::shared_ptr<Expr> expression;
};

struct LiteralExpr : Expr, public std::enable_shared_from_this<LiteralExpr> {
    LiteralExpr(std::variant<std::string, double, bool, nullptr_t> value)
        : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitLiteralExpr(shared_from_this());
    }

    const std::variant<std::string, double, bool, nullptr_t> value;
};

struct LogicalExpr : Expr, public std::enable_shared_from_this<LogicalExpr> {
    LogicalExpr(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitLogicalExpr(shared_from_this());
    }

    const std::shared_ptr<Expr> left;
    const Token op;
    const std::shared_ptr<Expr> right;
};

struct UnaryExpr : Expr, public std::enable_shared_from_this<UnaryExpr> {
    UnaryExpr(Token op, std::shared_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitUnaryExpr(shared_from_this());
    }

    const Token op;
    const std::shared_ptr<Expr> right;
};

struct VariableExpr : Expr, public std::enable_shared_from_this<VariableExpr> {
    VariableExpr(Token name) : name(std::move(name)) {}

    std::any accept(ExprVisitor& visitor) override {
        return visitor.visitVariableExpr(shared_from_this());
    }

    const Token name;
};

// Forward declarations for Statements
struct BlockStmt;
struct ExpressionStmt;
struct FunctionStmt;
struct IfStmt;
struct LetStmt;
struct ReturnStmt;
struct WhileStmt;

// Visitor for Statements
class StmtVisitor {
public:
    virtual std::any visitBlockStmt(std::shared_ptr<BlockStmt> stmt) = 0;
    virtual std::any visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) = 0;
    virtual std::any visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) = 0;
    virtual std::any visitIfStmt(std::shared_ptr<IfStmt> stmt) = 0;
    virtual std::any visitLetStmt(std::shared_ptr<LetStmt> stmt) = 0;
    virtual std::any visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) = 0;
    virtual std::any visitWhileStmt(std::shared_ptr<WhileStmt> stmt) = 0;
    virtual ~StmtVisitor() = default;
};

// Base class for all statements
struct Stmt {
    virtual std::any accept(StmtVisitor& visitor) = 0;
};

// Statement nodes
struct BlockStmt : Stmt, public std::enable_shared_from_this<BlockStmt> {
    BlockStmt(std::vector<std::shared_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitBlockStmt(shared_from_this());
    }

    const std::vector<std::shared_ptr<Stmt>> statements;
};

struct ExpressionStmt : Stmt, public std::enable_shared_from_this<ExpressionStmt> {
    ExpressionStmt(std::shared_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitExpressionStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> expression;
};

struct FunctionStmt : Stmt, public std::enable_shared_from_this<FunctionStmt> {
    FunctionStmt(Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitFunctionStmt(shared_from_this());
    }

    const Token name;
    const std::vector<Token> params;
    const std::vector<std::shared_ptr<Stmt>> body;
};

struct IfStmt : Stmt, public std::enable_shared_from_this<IfStmt> {
    IfStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitIfStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> condition;
    const std::shared_ptr<Stmt> thenBranch;
    const std::shared_ptr<Stmt> elseBranch;
};

struct LetStmt : Stmt, public std::enable_shared_from_this<LetStmt> {
    LetStmt(Token name, std::shared_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitLetStmt(shared_from_this());
    }

    const Token name;
    const std::shared_ptr<Expr> initializer;
};

struct ReturnStmt : Stmt, public std::enable_shared_from_this<ReturnStmt> {
    ReturnStmt(Token keyword, std::shared_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitReturnStmt(shared_from_this());
    }

    const Token keyword;
    const std::shared_ptr<Expr> value;
};

struct WhileStmt : Stmt, public std::enable_shared_from_this<WhileStmt> {
    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::any accept(StmtVisitor& visitor) override {
        return visitor.visitWhileStmt(shared_from_this());
    }

    const std::shared_ptr<Expr> condition;
    const std::shared_ptr<Stmt> body;
};

} // namespace chtholly
