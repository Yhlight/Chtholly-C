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
struct SwitchStmt;
struct BreakStmt;
struct FallthroughStmt;
struct EnumStmt;

class StmtVisitor {
public:
    virtual std::string visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual std::string visitLetStmt(const LetStmt& stmt) = 0;
    virtual std::string visitBlockStmt(const BlockStmt& stmt) = 0;
    virtual std::string visitIfStmt(const IfStmt& stmt) = 0;
    virtual std::string visitWhileStmt(const WhileStmt& stmt) = 0;
    virtual std::string visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName = std::nullopt) = 0;
    virtual std::string visitReturnStmt(const ReturnStmt& stmt) = 0;
    virtual std::string visitStructStmt(const StructStmt& stmt) = 0;
    virtual std::string visitTraitStmt(const TraitStmt& stmt) = 0;
    virtual std::string visitImplStmt(const ImplStmt& stmt) = 0;
    virtual std::string visitImportStmt(const ImportStmt& stmt) = 0;
    virtual std::string visitSwitchStmt(const SwitchStmt& stmt) = 0;
    virtual std::string visitBreakStmt(const BreakStmt& stmt) = 0;
    virtual std::string visitFallthroughStmt(const FallthroughStmt& stmt) = 0;
    virtual std::string visitEnumStmt(const EnumStmt& stmt) = 0;
    virtual ~StmtVisitor() = default;
};

class Stmt {
public:
    virtual std::string accept(StmtVisitor& visitor) const = 0;
    virtual ~Stmt();
};

struct ExpressionStmt : Stmt {
    std::unique_ptr<Expr> expression;

    explicit ExpressionStmt(std::unique_ptr<Expr> expression);
    ~ExpressionStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitExpressionStmt(*this);
    }
};

struct LetStmt : Stmt {
    Token name;
    std::optional<TypeInfo> type;
    std::unique_ptr<Expr> initializer;
    bool isMutable;
    bool is_public = true;

    LetStmt(Token name, std::optional<TypeInfo> type, std::unique_ptr<Expr> initializer, bool isMutable, bool is_public = true);
    ~LetStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitLetStmt(*this);
    }
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements);
    ~BlockStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitBlockStmt(*this);
    }
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;

    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch);
    ~IfStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitIfStmt(*this);
    }
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body);
    ~WhileStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitWhileStmt(*this);
    }
};

struct FunctionStmt : Stmt {
    Token name;
    std::vector<Token> generics;
    std::vector<std::pair<Token, TypeInfo>> params;
    std::vector<std::unique_ptr<Stmt>> body;
    std::optional<TypeInfo> returnType;

    FunctionStmt(Token name, std::vector<Token> generics, std::vector<std::pair<Token, TypeInfo>> params, std::vector<std::unique_ptr<Stmt>> body, std::optional<TypeInfo> returnType);
    ~FunctionStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitFunctionStmt(*this);
    }
};

struct ReturnStmt : Stmt {
    Token keyword;
    std::unique_ptr<Expr> value;

    ReturnStmt(Token keyword, std::unique_ptr<Expr> value);
    ~ReturnStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitReturnStmt(*this);
    }
};

struct StructStmt : Stmt {
    Token name;
    std::vector<std::unique_ptr<LetStmt>> fields;

    StructStmt(Token name, std::vector<std::unique_ptr<LetStmt>> fields);
    ~StructStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitStructStmt(*this);
    }
};

struct TraitStmt : Stmt {
    Token name;
    std::vector<Token> generics;
    std::vector<std::unique_ptr<FunctionStmt>> methods;

    TraitStmt(Token name, std::vector<Token> generics, std::vector<std::unique_ptr<Stmt>> raw_methods);
    ~TraitStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitTraitStmt(*this);
    }
};

struct ImplStmt : Stmt {
    Token structName;
    std::optional<Token> traitName;
    std::vector<TypeInfo> generics;
    std::vector<std::unique_ptr<FunctionStmt>> methods;

    ImplStmt(Token structName, std::optional<Token> traitName, std::vector<TypeInfo> generics, std::vector<std::unique_ptr<Stmt>> raw_methods);
    ~ImplStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitImplStmt(*this);
    }
};

struct ImportStmt : Stmt {
    Token path;
    bool is_std;

    explicit ImportStmt(Token path, bool is_std = false);
    ~ImportStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitImportStmt(*this);
    }
};

struct BreakStmt : Stmt {
    Token keyword;

    explicit BreakStmt(Token keyword);
    ~BreakStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitBreakStmt(*this);
    }
};

struct FallthroughStmt : Stmt {
    Token keyword;

    explicit FallthroughStmt(Token keyword);
    ~FallthroughStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitFallthroughStmt(*this);
    }
};

struct CaseStmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;

    CaseStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body);
    ~CaseStmt();
    CaseStmt(CaseStmt&&) = default;
    CaseStmt& operator=(CaseStmt&&) = default;
    CaseStmt(const CaseStmt&) = delete;
    CaseStmt& operator=(const CaseStmt&) = delete;
};

struct SwitchStmt : Stmt {
    std::unique_ptr<Expr> expression;
    std::vector<CaseStmt> cases;
    std::optional<CaseStmt> defaultCase;

    SwitchStmt(std::unique_ptr<Expr> expression, std::vector<CaseStmt> cases, std::optional<CaseStmt> defaultCase);
    ~SwitchStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitSwitchStmt(*this);
    }
};

struct EnumStmt : Stmt {
    Token name;
    std::vector<Token> members;

    EnumStmt(Token name, std::vector<Token> members);
    ~EnumStmt() override;

    std::string accept(StmtVisitor& visitor) const override {
        return visitor.visitEnumStmt(*this);
    }
};
