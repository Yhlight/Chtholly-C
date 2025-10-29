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
class BooleanLiteralExpr;
class UnaryExpr;
class VariableExpr;
class VarDeclStmt;
class BlockStmt;
class ExprStmt;
class IfStmt;
class WhileStmt;
class SwitchStmt;
class CaseStmt;
class BreakStmt;
class FallthroughStmt;
class FunctionStmt;
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
    virtual std::any visitBooleanLiteralExpr(BooleanLiteralExpr& expr) = 0;
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
    virtual std::any visitIfStmt(IfStmt& stmt) = 0;
    virtual std::any visitWhileStmt(WhileStmt& stmt) = 0;
    virtual std::any visitSwitchStmt(SwitchStmt& stmt) = 0;
    virtual std::any visitCaseStmt(CaseStmt& stmt) = 0;
    virtual std::any visitBreakStmt(BreakStmt& stmt) = 0;
    virtual std::any visitFallthroughStmt(FallthroughStmt& stmt) = 0;
    virtual std::any visitFunctionStmt(FunctionStmt& stmt) = 0;
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

class BooleanLiteralExpr : public Expr
{
public:
    BooleanLiteralExpr(Token value) : value(std::move(value)) {}
    BooleanLiteralExpr(bool val) {
        value.type = TokenType::Boolean;
        value.value = val;
    }

    Token value;

    std::any accept(ExprVisitor& visitor) override { return visitor.visitBooleanLiteralExpr(*this); }
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

class IfStmt : public Stmt
{
public:
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    std::any accept(StmtVisitor& visitor) override { return visitor.visitIfStmt(*this); }
};

class WhileStmt : public Stmt
{
public:
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    std::any accept(StmtVisitor& visitor) override { return visitor.visitWhileStmt(*this); }
};

class SwitchStmt : public Stmt
{
public:
    SwitchStmt(std::unique_ptr<Expr> expression, std::vector<std::unique_ptr<Stmt>> cases)
        : expression(std::move(expression)), cases(std::move(cases)) {}

    std::unique_ptr<Expr> expression;
    std::vector<std::unique_ptr<Stmt>> cases;

    std::any accept(StmtVisitor& visitor) override { return visitor.visitSwitchStmt(*this); }
};

class CaseStmt : public Stmt
{
public:
    CaseStmt(std::unique_ptr<Expr> expression, std::unique_ptr<Stmt> body)
        : expression(std::move(expression)), body(std::move(body)) {}

    std::unique_ptr<Expr> expression;
    std::unique_ptr<Stmt> body;

    std::any accept(StmtVisitor& visitor) override { return visitor.visitCaseStmt(*this); }
};

class BreakStmt : public Stmt
{
public:
    BreakStmt() = default;
    std::any accept(StmtVisitor& visitor) override { return visitor.visitBreakStmt(*this); }
};

class FallthroughStmt : public Stmt
{
public:
    FallthroughStmt() = default;
    std::any accept(StmtVisitor& visitor) override { return visitor.visitFallthroughStmt(*this); }
};

class FunctionStmt : public Stmt
{
public:
    struct Parameter {
        Token name;
        Token type;
    };

    FunctionStmt(Token name, std::vector<Parameter> params, Token returnType, std::unique_ptr<Stmt> body)
        : name(std::move(name)), params(std::move(params)), returnType(std::move(returnType)), body(std::move(body)) {}

    Token name;
    std::vector<Parameter> params;
    Token returnType;
    std::unique_ptr<Stmt> body;

    std::any accept(StmtVisitor& visitor) override { return visitor.visitFunctionStmt(*this); }
};


} // namespace Chtholly

#endif // CHTHOLLY_AST_H
