#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "Token.h"
#include "TypeInfo.h"
#include <vector>
#include <memory>
#include <any>
#include <utility>
#include <map>

namespace chtholly {

// Forward declarations for all AST node types
struct BinaryExpr;
struct UnaryExpr;
struct LiteralExpr;
struct GroupingExpr;
struct VariableExpr;
struct AssignExpr;
struct CallExpr;
struct LambdaExpr;
struct GetExpr;
struct SetExpr;
struct SelfExpr;
struct BorrowExpr;
struct DerefExpr;
struct StructLiteralExpr;
struct ArrayLiteralExpr;
struct TypeCastExpr;

struct TypeExpr;
struct BaseTypeExpr;
struct ArrayTypeExpr;
struct FunctionTypeExpr;
struct GenericTypeExpr;
struct BorrowTypeExpr;

struct Stmt;
struct BlockStmt;
struct ExpressionStmt;
struct FunctionStmt;
struct IfStmt;
struct VarStmt;
struct WhileStmt;
struct ForStmt;
struct ReturnStmt;
struct StructStmt;
struct ImportStmt;
struct SwitchStmt;
struct CaseStmt;
struct BreakStmt;
struct FallthroughStmt;
struct EnumStmt;
struct TraitStmt;

// Visitor for Expressions
class ExprVisitor {
public:
    virtual std::any visitBinaryExpr(const BinaryExpr& expr) = 0;
    virtual std::any visitUnaryExpr(const UnaryExpr& expr) = 0;
    virtual std::any visitLiteralExpr(const LiteralExpr& expr) = 0;
    virtual std::any visitGroupingExpr(const GroupingExpr& expr) = 0;
    virtual std::any visitVariableExpr(const VariableExpr& expr) = 0;
    virtual std::any visitAssignExpr(const AssignExpr& expr) = 0;
    virtual std::any visitCallExpr(const CallExpr& expr) = 0;
    virtual std::any visitLambdaExpr(const LambdaExpr& expr) = 0;
    virtual std::any visitGetExpr(const GetExpr& expr) = 0;
    virtual std::any visitSetExpr(const SetExpr& expr) = 0;
    virtual std::any visitSelfExpr(const SelfExpr& expr) = 0;
    virtual std::any visitBorrowExpr(const BorrowExpr& expr) = 0;
    virtual std::any visitDerefExpr(const DerefExpr& expr) = 0;
    virtual std::any visitStructLiteralExpr(const StructLiteralExpr& expr) = 0;
    virtual std::any visitArrayLiteralExpr(const ArrayLiteralExpr& expr) = 0;
    virtual std::any visitTypeCastExpr(const TypeCastExpr& expr) = 0;
};

// Base class for all expressions
struct Expr {
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) const = 0;
};

// Visitor for Statements
class StmtVisitor {
public:
    virtual std::any visitBlockStmt(const BlockStmt& stmt) = 0;
    virtual std::any visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual std::any visitFunctionStmt(const FunctionStmt& stmt) = 0;
    virtual std::any visitIfStmt(const IfStmt& stmt) = 0;
    virtual std::any visitVarStmt(const VarStmt& stmt) = 0;
    virtual std::any visitWhileStmt(const WhileStmt& stmt) = 0;
    virtual std::any visitForStmt(const ForStmt& stmt) = 0;
    virtual std::any visitReturnStmt(const ReturnStmt& stmt) = 0;
    virtual std::any visitStructStmt(const StructStmt& stmt) = 0;
    virtual std::any visitImportStmt(const ImportStmt& stmt) = 0;
    virtual std::any visitSwitchStmt(const SwitchStmt& stmt) = 0;
    virtual std::any visitCaseStmt(const CaseStmt& stmt) = 0;
    virtual std::any visitBreakStmt(const BreakStmt& stmt) = 0;
    virtual std::any visitFallthroughStmt(const FallthroughStmt& stmt) = 0;
    virtual std::any visitEnumStmt(const EnumStmt& stmt) = 0;
    virtual std::any visitTraitStmt(const TraitStmt& stmt) = 0;
};

// Base class for all statements
enum class Access { PUBLIC, PRIVATE };

struct Stmt {
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor& visitor) const = 0;
};

// Concrete Expression Nodes
struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitBinaryExpr(*this); }
};

struct UnaryExpr : Expr {
    Token op;
    std::unique_ptr<Expr> right;
    bool is_postfix;
    UnaryExpr(Token op, std::unique_ptr<Expr> right, bool is_postfix = false)
        : op(std::move(op)), right(std::move(right)), is_postfix(is_postfix) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitUnaryExpr(*this); }
};

struct LiteralExpr : Expr {
    std::variant<std::nullptr_t, std::string, double, long long, bool, char> value;
    explicit LiteralExpr(std::variant<std::nullptr_t, std::string, double, long long, bool, char> value) : value(std::move(value)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitLiteralExpr(*this); }
};

struct GroupingExpr : Expr {
    std::unique_ptr<Expr> expression;
    explicit GroupingExpr(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitGroupingExpr(*this); }
};

struct VariableExpr : Expr {
    Token name;
    explicit VariableExpr(Token name) : name(std::move(name)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitVariableExpr(*this); }
};

struct AssignExpr : Expr {
    Token name;
    std::unique_ptr<Expr> value;
    AssignExpr(Token name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitAssignExpr(*this); }
};

struct CallExpr : Expr {
    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<TypeExpr>> generic_args;
    std::vector<std::unique_ptr<Expr>> arguments;
    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<TypeExpr>> generic_args, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), generic_args(std::move(generic_args)), arguments(std::move(arguments)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitCallExpr(*this); }
};

struct LambdaExpr : Expr {
    std::vector<Token> captures;
    std::vector<Token> params;
    std::vector<std::unique_ptr<TypeExpr>> param_types;
    std::unique_ptr<TypeExpr> return_type;
    std::unique_ptr<BlockStmt> body;

    LambdaExpr(std::vector<Token> captures, std::vector<Token> params, std::vector<std::unique_ptr<TypeExpr>> param_types, std::unique_ptr<TypeExpr> return_type, std::unique_ptr<BlockStmt> body)
        : captures(std::move(captures)), params(std::move(params)), param_types(std::move(param_types)), return_type(std::move(return_type)), body(std::move(body)) {}

    std::any accept(ExprVisitor& visitor) const override { return visitor.visitLambdaExpr(*this); }
};

struct GetExpr : Expr {
    std::unique_ptr<Expr> object;
    Token name;
    GetExpr(std::unique_ptr<Expr> object, Token name)
        : object(std::move(object)), name(std::move(name)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitGetExpr(*this); }
};

struct SetExpr : Expr {
    std::unique_ptr<Expr> object;
    Token name;
    std::unique_ptr<Expr> value;
    SetExpr(std::unique_ptr<Expr> object, Token name, std::unique_ptr<Expr> value)
        : object(std::move(object)), name(std::move(name)), value(std::move(value)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitSetExpr(*this); }
};

struct SelfExpr : Expr {
    Token keyword;
    explicit SelfExpr(Token keyword) : keyword(std::move(keyword)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitSelfExpr(*this); }
};

struct BorrowExpr : Expr {
    std::unique_ptr<Expr> expression;
    bool isMutable;
    BorrowExpr(std::unique_ptr<Expr> expression, bool isMutable)
        : expression(std::move(expression)), isMutable(isMutable) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitBorrowExpr(*this); }
};

struct DerefExpr : Expr {
    std::unique_ptr<Expr> expression;
    explicit DerefExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitDerefExpr(*this); }
};

struct StructLiteralExpr : Expr {
    Token name;
    std::variant<
        std::map<std::string, std::unique_ptr<Expr>>, // Named fields
        std::vector<std::unique_ptr<Expr>>           // Positional fields
    > initializers;
    StructLiteralExpr(Token name, std::variant<std::map<std::string, std::unique_ptr<Expr>>, std::vector<std::unique_ptr<Expr>>> initializers)
        : name(std::move(name)), initializers(std::move(initializers)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitStructLiteralExpr(*this); }
};

struct ArrayLiteralExpr : Expr {
    std::vector<std::unique_ptr<Expr>> elements;
    explicit ArrayLiteralExpr(std::vector<std::unique_ptr<Expr>> elements)
        : elements(std::move(elements)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitArrayLiteralExpr(*this); }
};

struct TypeCastExpr : Expr {
    std::unique_ptr<TypeExpr> type;
    std::unique_ptr<Expr> expression;
    TypeCastExpr(std::unique_ptr<TypeExpr> type, std::unique_ptr<Expr> expression)
        : type(std::move(type)), expression(std::move(expression)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitTypeCastExpr(*this); }
};


// Concrete Statement Nodes
struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) : statements(std::move(statements)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitBlockStmt(*this); }
};

struct ExpressionStmt : Stmt {
    std::unique_ptr<Expr> expression;
    explicit ExpressionStmt(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitExpressionStmt(*this); }
};

struct FunctionStmt : Stmt {
    Token name;
    std::vector<Token> generic_params;
    std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>> generic_constraints;
    std::vector<Token> params;
    std::vector<std::unique_ptr<TypeExpr>> param_types;
    std::unique_ptr<TypeExpr> return_type;
    std::unique_ptr<BlockStmt> body;
    Access access;
    std::unique_ptr<TypeExpr> trait_impl; // For internal `impl Trait` on methods
    FunctionStmt(Token name, std::vector<Token> generic_params, std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>> generic_constraints, std::vector<Token> params, std::vector<std::unique_ptr<TypeExpr>> param_types, std::unique_ptr<TypeExpr> return_type, std::unique_ptr<BlockStmt> body, Access access = Access::PUBLIC, std::unique_ptr<TypeExpr> trait_impl = nullptr)
        : name(std::move(name)), generic_params(std::move(generic_params)), generic_constraints(std::move(generic_constraints)), params(std::move(params)), param_types(std::move(param_types)), return_type(std::move(return_type)), body(std::move(body)), access(access), trait_impl(std::move(trait_impl)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitFunctionStmt(*this); }
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitIfStmt(*this); }
};

struct VarStmt : Stmt {
    Token name;
    std::unique_ptr<TypeExpr> type;
    std::unique_ptr<Expr> initializer;
    bool isMutable;
    Access access;
    VarStmt(Token name, std::unique_ptr<TypeExpr> type, std::unique_ptr<Expr> initializer, bool isMutable, Access access = Access::PUBLIC)
        : name(std::move(name)), type(std::move(type)), initializer(std::move(initializer)), isMutable(isMutable), access(access) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitVarStmt(*this); }
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitWhileStmt(*this); }
};

struct ForStmt : Stmt {
    std::unique_ptr<Stmt> initializer;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> increment;
    std::unique_ptr<Stmt> body;
    ForStmt(std::unique_ptr<Stmt> initializer, std::unique_ptr<Expr> condition, std::unique_ptr<Expr> increment, std::unique_ptr<Stmt> body)
        : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitForStmt(*this); }
};

struct ReturnStmt : Stmt {
    Token keyword;
    std::unique_ptr<Expr> value;
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitReturnStmt(*this); }
};

struct StructStmt : Stmt {
    Token name;
    std::vector<Token> generic_params;
    std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>> generic_constraints;
    std::vector<std::unique_ptr<Expr>> traits;
    std::vector<std::unique_ptr<VarStmt>> fields;
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    StructStmt(Token name, std::vector<Token> generic_params, std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>> generic_constraints, std::vector<std::unique_ptr<Expr>> traits, std::vector<std::unique_ptr<VarStmt>> fields, std::vector<std::unique_ptr<FunctionStmt>> methods)
        : name(std::move(name)), generic_params(std::move(generic_params)), generic_constraints(std::move(generic_constraints)), traits(std::move(traits)), fields(std::move(fields)), methods(std::move(methods)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitStructStmt(*this); }
};

struct ImportStmt : Stmt {
    Token keyword;
    std::variant<std::string, Token> path; // string for file path, Token for module
    ImportStmt(Token keyword, std::variant<std::string, Token> path)
        : keyword(std::move(keyword)), path(std::move(path)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitImportStmt(*this); }
};

struct SwitchStmt : Stmt {
    std::unique_ptr<Expr> expression;
    std::vector<std::unique_ptr<CaseStmt>> cases;
    SwitchStmt(std::unique_ptr<Expr> expression, std::vector<std::unique_ptr<CaseStmt>> cases)
        : expression(std::move(expression)), cases(std::move(cases)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitSwitchStmt(*this); }
};

struct CaseStmt : Stmt {
    std::unique_ptr<Expr> value;
    std::unique_ptr<Stmt> body;
    CaseStmt(std::unique_ptr<Expr> value, std::unique_ptr<Stmt> body)
        : value(std::move(value)), body(std::move(body)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitCaseStmt(*this); }
};

struct BreakStmt : Stmt {
    Token keyword;
    explicit BreakStmt(Token keyword) : keyword(std::move(keyword)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitBreakStmt(*this); }
};

struct FallthroughStmt : Stmt {
    Token keyword;
    explicit FallthroughStmt(Token keyword) : keyword(std::move(keyword)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitFallthroughStmt(*this); }
};

struct EnumStmt : Stmt {
    Token name;
    std::vector<Token> members;
    EnumStmt(Token name, std::vector<Token> members)
        : name(std::move(name)), members(std::move(members)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitEnumStmt(*this); }
};

struct TraitStmt : Stmt {
    Token name;
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    TraitStmt(Token name, std::vector<std::unique_ptr<FunctionStmt>> methods)
        : name(std::move(name)), methods(std::move(methods)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitTraitStmt(*this); }
};


// Type Expression Nodes
struct TypeExpr {
    virtual ~TypeExpr() = default;
};

struct BaseTypeExpr : TypeExpr {
    Token type;
    explicit BaseTypeExpr(Token type) : type(std::move(type)) {}
};

struct ArrayTypeExpr : TypeExpr {
    std::unique_ptr<TypeExpr> element_type;
    std::unique_ptr<Expr> size; // optional
    ArrayTypeExpr(std::unique_ptr<TypeExpr> element_type, std::unique_ptr<Expr> size)
        : element_type(std::move(element_type)), size(std::move(size)) {}
};

struct FunctionTypeExpr : TypeExpr {
    std::vector<std::unique_ptr<TypeExpr>> param_types;
    std::unique_ptr<TypeExpr> return_type;
    FunctionTypeExpr(std::vector<std::unique_ptr<TypeExpr>> param_types, std::unique_ptr<TypeExpr> return_type)
        : param_types(std::move(param_types)), return_type(std::move(return_type)) {}
};

struct GenericTypeExpr : TypeExpr {
    Token base_type;
    std::vector<std::unique_ptr<TypeExpr>> generic_args;
    GenericTypeExpr(Token base_type, std::vector<std::unique_ptr<TypeExpr>> generic_args)
        : base_type(std::move(base_type)), generic_args(std::move(generic_args)) {}
};

struct BorrowTypeExpr : TypeExpr {
    std::unique_ptr<TypeExpr> element_type;
    bool isMutable;
    BorrowTypeExpr(std::unique_ptr<TypeExpr> element_type, bool isMutable)
        : element_type(std::move(element_type)), isMutable(isMutable) {}
};

} // namespace chtholly

#endif // CHTHOLLY_AST_H
