#pragma once

#include "../TypeInfo.h"
#include <stdexcept>
#include <any>
#include <memory>
#include <vector>
#include <optional>

class Expr;

struct ExpressionStmt;
struct LetStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct FunctionStmt;
struct ReturnStmt;
struct StructStmt;
struct TraitStmt;
struct ImplStmt;
struct ImportStmt;

class StmtVisitor {
public:
    virtual std::any visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual std::any visitLetStmt(const LetStmt& stmt) = 0;
    virtual std::any visitBlockStmt(const BlockStmt& stmt) = 0;
    virtual std::any visitIfStmt(const IfStmt& stmt) = 0;
    virtual std::any visitWhileStmt(const WhileStmt& stmt) = 0;
    virtual std::any visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName = std::nullopt) = 0;
    virtual std::any visitReturnStmt(const ReturnStmt& stmt) = 0;
    virtual std::any visitStructStmt(const StructStmt& stmt) = 0;
    virtual std::any visitTraitStmt(const TraitStmt& stmt) = 0;
    virtual std::any visitImplStmt(const ImplStmt& stmt) = 0;
    virtual std::any visitImportStmt(const ImportStmt& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

class Stmt {
public:
    virtual std::any accept(StmtVisitor& visitor) const = 0;
    virtual ~Stmt() = default;
};

struct ExpressionStmt : Stmt {
    std::unique_ptr<Expr> expression;

    explicit ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitExpressionStmt(*this);
    }
};

struct LetStmt : Stmt {
    Token name;
    std::optional<TypeInfo> type;
    std::unique_ptr<Expr> initializer;
    bool isMutable;

    LetStmt(Token name, std::optional<TypeInfo> type, std::unique_ptr<Expr> initializer, bool isMutable)
        : name(name), type(std::move(type)), initializer(std::move(initializer)), isMutable(isMutable) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitLetStmt(*this);
    }
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitBlockStmt(*this);
    }
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitIfStmt(*this);
    }
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitWhileStmt(*this);
    }
};

struct FunctionStmt : Stmt {
    Token name;
    std::vector<Token> generics;
    std::vector<std::pair<Token, TypeInfo>> params;
    std::vector<std::unique_ptr<Stmt>> body;
    std::optional<TypeInfo> returnType;

    FunctionStmt(Token name, std::vector<Token> generics, std::vector<std::pair<Token, TypeInfo>> params, std::vector<std::unique_ptr<Stmt>> body, std::optional<TypeInfo> returnType)
        : name(name), generics(std::move(generics)), params(std::move(params)), body(std::move(body)), returnType(std::move(returnType)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitFunctionStmt(*this);
    }
};

struct ReturnStmt : Stmt {
    Token keyword;
    std::unique_ptr<Expr> value;

    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(keyword), value(std::move(value)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitReturnStmt(*this);
    }
};

struct StructStmt : Stmt {
    Token name;
    std::vector<std::unique_ptr<LetStmt>> fields;

    StructStmt(Token name, std::vector<std::unique_ptr<LetStmt>> fields)
        : name(name), fields(std::move(fields)) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitStructStmt(*this);
    }
};

struct TraitStmt : Stmt {
    Token name;
    std::vector<Token> generics;
    std::vector<std::unique_ptr<FunctionStmt>> methods;

    TraitStmt(Token name, std::vector<Token> generics, std::vector<std::unique_ptr<Stmt>> raw_methods)
        : name(name), generics(std::move(generics)) {
        for (auto& stmt : raw_methods) {
            auto func_stmt = dynamic_cast<FunctionStmt*>(stmt.get());
            if (func_stmt) {
                stmt.release();
                methods.emplace_back(func_stmt);
            } else {
                throw std::runtime_error("Trait method is not a FunctionStmt");
            }
        }
    }

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitTraitStmt(*this);
    }
};

struct ImplStmt : Stmt {
    Token structName;
    std::optional<Token> traitName;
    std::vector<TypeInfo> generics;
    std::vector<std::unique_ptr<FunctionStmt>> methods;

    ImplStmt(Token structName, std::optional<Token> traitName, std::vector<TypeInfo> generics, std::vector<std::unique_ptr<Stmt>> raw_methods)
        : structName(structName), traitName(traitName), generics(std::move(generics)) {
        for (auto& stmt : raw_methods) {
            auto func_stmt = dynamic_cast<FunctionStmt*>(stmt.get());
            if (func_stmt) {
                stmt.release();
                methods.emplace_back(func_stmt);
            } else {
                throw std::runtime_error("Impl method is not a FunctionStmt");
            }
        }
    }

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitImplStmt(*this);
    }
};

struct ImportStmt : Stmt {
    Token path;
    bool is_std;

    explicit ImportStmt(Token path, bool is_std = false)
        : path(path), is_std(is_std) {}

    std::any accept(StmtVisitor& visitor) const override {
        return visitor.visitImportStmt(*this);
    }
};
