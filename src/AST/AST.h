#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "../Token.h"
#include <memory>
#include <vector>
#include <any>

namespace Chtholly
{

// Forward declarations
class NumberLiteralExpr;
class VarDeclStmt;
class BlockStmt;
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
    virtual std::any visitNumberLiteralExpr(NumberLiteralExpr& expr) = 0;
};

class StmtVisitor
{
public:
    virtual ~StmtVisitor() = default;
    virtual std::any visitVarDeclStmt(VarDeclStmt& stmt) = 0;
    virtual std::any visitBlockStmt(BlockStmt& stmt) = 0;
};

// Expression for a number literal
class NumberLiteralExpr : public Expr
{
public:
    NumberLiteralExpr(Token number) : number(std::move(number)) {}

    Token number;

    std::any accept(ExprVisitor& visitor) override
    {
        return visitor.visitNumberLiteralExpr(*this);
    }
};

// Statement for a variable declaration
class VarDeclStmt : public Stmt
{
public:
    VarDeclStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    Token name;
    std::unique_ptr<Expr> initializer;

    std::any accept(StmtVisitor& visitor) override
    {
        return visitor.visitVarDeclStmt(*this);
    }
};

// Statement for a block of statements
class BlockStmt : public Stmt
{
public:
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::vector<std::unique_ptr<Stmt>> statements;

    std::any accept(StmtVisitor& visitor) override
    {
        return visitor.visitBlockStmt(*this);
    }
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
