#pragma once

#include "../Token.h"
#include <any>
#include <memory>
#include <vector>
#include <optional>
#include <map>

#include "../TypeInfo.h"
class Stmt;

struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct UnaryExpr;
struct VariableExpr;
struct AssignExpr;
struct CallExpr;
struct GetExpr;
struct SetExpr;
struct BorrowExpr;
struct LambdaExpr;
struct StructInitializerExpr;

class ExprVisitor {
public:
    virtual std::string visitBinaryExpr(const BinaryExpr& expr) = 0;
    virtual std::string visitGroupingExpr(const GroupingExpr& expr) = 0;
    virtual std::string visitLiteralExpr(const LiteralExpr& expr) = 0;
    virtual std::string visitUnaryExpr(const UnaryExpr& expr) = 0;
    virtual std::string visitVariableExpr(const VariableExpr& expr) = 0;
    virtual std::string visitAssignExpr(const AssignExpr& expr) = 0;
    virtual std::string visitCallExpr(const CallExpr& expr) = 0;
    virtual std::string visitGetExpr(const GetExpr& expr) = 0;
    virtual std::string visitSetExpr(const SetExpr& expr) = 0;
    virtual std::string visitBorrowExpr(const BorrowExpr& expr) = 0;
    virtual std::string visitLambdaExpr(const LambdaExpr& expr) = 0;
    virtual std::string visitStructInitializerExpr(const StructInitializerExpr& expr) = 0;
    virtual ~ExprVisitor() = default;
};

class Expr {
public:
    virtual std::string accept(ExprVisitor& visitor) const = 0;
    virtual ~Expr();
    mutable std::optional<TypeInfo> resolved_type;
};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right);
    ~BinaryExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }
};

struct GroupingExpr : Expr {
    std::unique_ptr<Expr> expression;

    explicit GroupingExpr(std::unique_ptr<Expr> expression);
    ~GroupingExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }
};

struct LiteralExpr : Expr {
    std::variant<std::monostate, std::string, double, bool> value;

    explicit LiteralExpr(std::variant<std::monostate, std::string, double, bool> value);
    ~LiteralExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }
};

struct UnaryExpr : Expr {
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Token op, std::unique_ptr<Expr> right);
    ~UnaryExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }
};

struct VariableExpr : Expr {
    Token name;

    explicit VariableExpr(Token name);
    ~VariableExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitVariableExpr(*this);
    }
};

struct AssignExpr : Expr {
    Token name;
    std::unique_ptr<Expr> value;

    AssignExpr(Token name, std::unique_ptr<Expr> value);
    ~AssignExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitAssignExpr(*this);
    }
};

struct CallExpr : Expr {
    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<Expr>> arguments;
    std::vector<TypeInfo> generic_args;

    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments, std::vector<TypeInfo> generic_args);
    ~CallExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitCallExpr(*this);
    }
};

struct GetExpr : Expr {
    std::unique_ptr<Expr> object;
    Token name;

    GetExpr(std::unique_ptr<Expr> object, Token name);
    ~GetExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitGetExpr(*this);
    }
};

struct SetExpr : Expr {
    std::unique_ptr<Expr> object;
    Token name;
    std::unique_ptr<Expr> value;

    SetExpr(std::unique_ptr<Expr> object, Token name, std::unique_ptr<Expr> value);
    ~SetExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitSetExpr(*this);
    }
};

struct BorrowExpr : Expr {
    std::unique_ptr<Expr> expression;
    bool isMutable;

    BorrowExpr(std::unique_ptr<Expr> expression, bool isMutable);
    ~BorrowExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitBorrowExpr(*this);
    }
};

struct LambdaExpr : Expr {
    std::vector<std::pair<Token, TypeInfo>> params;
    std::vector<std::unique_ptr<Stmt>> body;
    std::optional<TypeInfo> returnType;

    LambdaExpr(std::vector<std::pair<Token, TypeInfo>> params, std::vector<std::unique_ptr<Stmt>> body, std::optional<TypeInfo> returnType);
    ~LambdaExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitLambdaExpr(*this);
    }
};

struct StructInitializerExpr : Expr {
    Token name;
    std::map<Token, std::unique_ptr<Expr>> initializers;

    StructInitializerExpr(Token name, std::map<Token, std::unique_ptr<Expr>> initializers);
    ~StructInitializerExpr() override;

    std::string accept(ExprVisitor& visitor) const override {
        return visitor.visitStructInitializerExpr(*this);
    }
};
