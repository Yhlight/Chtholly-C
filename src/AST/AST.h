#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "../Token.h"
#include <memory>
#include <vector>
#include <any>

namespace Chtholly
{

// Forward declarations
class AssignExpr;
class BinaryExpr;
class GroupingExpr;
class NumberLiteralExpr;
class UnaryExpr;
class VariableExpr;
class VarDeclStmt;
class BlockStmt;
class ExprStmt;
class ExprVisitor;
class StmtVisitor;

// Base class for all expression nodes
class Expr
{
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) = 0;
};

// Base class for all statement nodes
class Stmt
{
public:
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor& visitor) = 0;
};

// Visitor interfaces
class ExprVisitor
{
public:
    virtual ~ExprVisitor() = default;
    virtual std::any visitAssignExpr(AssignExpr& expr) = 0;
    virtual std::any visitBinaryExpr(BinaryExpr& expr) = 0;
    virtual std::any visitGroupingExpr(GroupingExpr& expr) = 0;
    virtual std::any visitNumberLiteralExpr(NumberLiteralExpr& expr) = 0;
    virtual std::any visitUnaryExpr(UnaryExpr& expr) = 0;
    virtual std::any visitVariableExpr(VariableExpr& expr) = 0;
};

class StmtVisitor
{
public:
    virtual ~StmtVisitor() = default;
    virtual std::any visitVarDeclStmt(VarDeclStmt& stmt) = 0;
    virtual std::any visitBlockStmt(BlockStmt& stmt) = 0;
    virtual std::any visitExprStmt(ExprStmt& stmt) = 0;
};

// --- Expressions ---

class AssignExpr : public Expr
{
public:
    AssignExpr(Token name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}

    Token name;
    std::unique_ptr<Expr> value;

    std::any accept(ExprVisitor& visitor) override { return visitor.visitAssignExpr(*this); }
};

class BinaryExpr : public Expr
{
public:
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    std::any accept(ExprVisitor& visitor) override { return visitor.visitBinaryExpr(*this); }
};

class GroupingExpr : public Expr
{
public:
    GroupingExpr(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::unique_ptr<Expr> expression;

    std::any accept(ExprVisitor& visitor) override { return visitor.visitGroupingExpr(*this); }
};

class NumberLiteralExpr : public Expr
{
public:
    NumberLiteralExpr(Token number) : number(std::move(number)) {}

    Token number;

    std::any accept(ExprVisitor& visitor) override { return visitor.visitNumberLiteralExpr(*this); }
};

class UnaryExpr : public Expr
{
public:
    UnaryExpr(Token op, std::unique_ptr<Expr> right) : op(std::move(op)), right(std::move(right)) {}

    Token op;
    std::unique_ptr<Expr> right;

    std::any accept(ExprVisitor& visitor) override { return visitor.visitUnaryExpr(*this); }
};

class VariableExpr : public Expr
{
public:
    VariableExpr(Token name) : name(std::move(name)) {}

    Token name;

    std::any accept(ExprVisitor& visitor) override { return visitor.visitVariableExpr(*this); }
};

// --- Statements ---

class VarDeclStmt : public Stmt
{
public:
    VarDeclStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    Token name;
    std::unique_ptr<Expr> initializer;

    std::any accept(StmtVisitor& visitor) override { return visitor.visitVarDeclStmt(*this); }
};

class BlockStmt : public Stmt
{
public:
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::vector<std::unique_ptr<Stmt>> statements;

    std::any accept(StmtVisitor& visitor) override { return visitor.visitBlockStmt(*this); }
};

class ExprStmt : public Stmt
{
public:
    ExprStmt(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

    std::unique_ptr<Expr> expression;

    std::any accept(StmtVisitor& visitor) override { return visitor.visitExprStmt(*this); }
};


} // namespace Chtholly

#endif // CHTHOLLY_AST_H
