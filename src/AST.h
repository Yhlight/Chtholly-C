#pragma once

#include "Token.h"
#include <vector>
#include <memory>
#include <any>

namespace chtholly {

// Forward declarations for visitors
struct ExprVisitor;
struct StmtVisitor;
struct VarStmt;
struct FunctionStmt;
struct ReturnStmt;
struct CallExpr;
struct TypeVisitor;
struct BaseTypeExpr;
struct TypeExpr;

// Base class for all expression nodes
struct Expr {
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) = 0;
};

// Base class for all statement nodes
struct Stmt {
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor& visitor) = 0;
};

// Expression nodes
struct BinaryExpr : Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override;

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

struct UnaryExpr : Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) override;

    Token op;
    std::unique_ptr<Expr> right;
};

struct LiteralExpr : Expr {
    LiteralExpr(std::variant<std::monostate, std::string, double, bool> value)
        : value(value) {}

    std::any accept(ExprVisitor& visitor) override;

    std::variant<std::monostate, std::string, double, bool> value;
};

struct VariableExpr : Expr {
    VariableExpr(Token name) : name(name) {}

    std::any accept(ExprVisitor& visitor) override;

    Token name;
};

struct CallExpr : Expr {
    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}

    std::any accept(ExprVisitor& visitor) override;

    std::unique_ptr<Expr> callee;
    Token paren; // ')' for location
    std::vector<std::unique_ptr<Expr>> arguments;
};

// Visitor interface for expressions
struct ExprVisitor {
    virtual ~ExprVisitor() = default;
    virtual std::any visitBinaryExpr(BinaryExpr& expr) = 0;
    virtual std::any visitUnaryExpr(UnaryExpr& expr) = 0;
    virtual std::any visitLiteralExpr(LiteralExpr& expr) = 0;
    virtual std::any visitVariableExpr(VariableExpr& expr) = 0;
    virtual std::any visitCallExpr(CallExpr& expr) = 0;
};

// Statement nodes
struct ExpressionStmt : Stmt {
    ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(StmtVisitor& visitor) override;

    std::unique_ptr<Expr> expression;
};

struct BlockStmt : Stmt {
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::any accept(StmtVisitor& visitor) override;

    std::vector<std::unique_ptr<Stmt>> statements;
};

struct VarStmt : Stmt {
    VarStmt(Token keyword, Token name, std::unique_ptr<TypeExpr> type, std::unique_ptr<Expr> initializer)
        : keyword(keyword), name(name), type(std::move(type)), initializer(std::move(initializer)) {}

    std::any accept(StmtVisitor& visitor) override;

    Token keyword;
    Token name;
    std::unique_ptr<TypeExpr> type;
    std::unique_ptr<Expr> initializer;
};

// Visitor interface for statements
struct StmtVisitor {
    virtual ~StmtVisitor() = default;
    virtual std::any visitExpressionStmt(ExpressionStmt& stmt) = 0;
    virtual std::any visitBlockStmt(BlockStmt& stmt) = 0;
    virtual std::any visitVarStmt(VarStmt& stmt) = 0;
    virtual std::any visitFunctionStmt(FunctionStmt& stmt) = 0;
    virtual std::any visitReturnStmt(ReturnStmt& stmt) = 0;
};

struct FunctionStmt : Stmt {
    FunctionStmt(Token name, std::vector<Token> params, std::unique_ptr<TypeExpr> returnType, std::unique_ptr<BlockStmt> body)
        : name(name), params(std::move(params)), returnType(std::move(returnType)), body(std::move(body)) {}

    std::any accept(StmtVisitor& visitor) override;

    Token name;
    std::vector<Token> params;
    std::unique_ptr<TypeExpr> returnType;
    std::unique_ptr<BlockStmt> body;
};

struct ReturnStmt : Stmt {
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(keyword), value(std::move(value)) {}

    std::any accept(StmtVisitor& visitor) override;

    Token keyword;
    std::unique_ptr<Expr> value;
};

// Type expression nodes
struct TypeExpr {
    virtual ~TypeExpr() = default;
    virtual std::any accept(TypeVisitor& visitor) = 0;
};

struct BaseTypeExpr : TypeExpr {
    BaseTypeExpr(Token name) : name(name) {}

    std::any accept(TypeVisitor& visitor) override;

    Token name;
};

// Visitor interface for type expressions
struct TypeVisitor {
    virtual ~TypeVisitor() = default;
    virtual std::any visitBaseTypeExpr(BaseTypeExpr& expr) = 0;
};


// Implementations of accept methods
inline std::any BinaryExpr::accept(ExprVisitor& visitor) { return visitor.visitBinaryExpr(*this); }
inline std::any UnaryExpr::accept(ExprVisitor& visitor) { return visitor.visitUnaryExpr(*this); }
inline std::any LiteralExpr::accept(ExprVisitor& visitor) { return visitor.visitLiteralExpr(*this); }
inline std::any VariableExpr::accept(ExprVisitor& visitor) { return visitor.visitVariableExpr(*this); }
inline std::any CallExpr::accept(ExprVisitor& visitor) { return visitor.visitCallExpr(*this); }

inline std::any ExpressionStmt::accept(StmtVisitor& visitor) { return visitor.visitExpressionStmt(*this); }
inline std::any BlockStmt::accept(StmtVisitor& visitor) { return visitor.visitBlockStmt(*this); }
inline std::any VarStmt::accept(StmtVisitor& visitor) { return visitor.visitVarStmt(*this); }
inline std::any FunctionStmt::accept(StmtVisitor& visitor) { return visitor.visitFunctionStmt(*this); }
inline std::any ReturnStmt::accept(StmtVisitor& visitor) { return visitor.visitReturnStmt(*this); }

inline std::any BaseTypeExpr::accept(TypeVisitor& visitor) { return visitor.visitBaseTypeExpr(*this); }

} // namespace chtholly
