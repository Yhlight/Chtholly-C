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
struct CaseStmt;
struct SwitchStmt;
struct BreakStmt;
struct FallthroughStmt;
struct EnumStmt;
struct TraitStmt;

// Base class for all expressions
struct Expr {
    virtual ~Expr() = default;
    virtual std::any accept(class ExprVisitor& visitor) const = 0;
    virtual std::unique_ptr<Expr> clone() const = 0;
};

// Type Expression Nodes
struct TypeExpr {
    virtual ~TypeExpr() = default;
    virtual std::unique_ptr<TypeExpr> clone() const = 0;
};

struct BaseTypeExpr : TypeExpr {
    Token type;
    explicit BaseTypeExpr(Token type) : type(std::move(type)) {}
    std::unique_ptr<TypeExpr> clone() const override {
        return std::make_unique<BaseTypeExpr>(type);
    }
};

struct ArrayTypeExpr : TypeExpr {
    std::unique_ptr<TypeExpr> element_type;
    std::unique_ptr<Expr> size; // optional
    ArrayTypeExpr(std::unique_ptr<TypeExpr> element_type, std::unique_ptr<Expr> size)
        : element_type(std::move(element_type)), size(std::move(size)) {}
    std::unique_ptr<TypeExpr> clone() const override {
        return std::make_unique<ArrayTypeExpr>(element_type->clone(), size ? size->clone() : nullptr);
    }
};

struct FunctionTypeExpr : TypeExpr {
    std::vector<std::unique_ptr<TypeExpr>> param_types;
    std::unique_ptr<TypeExpr> return_type;
    FunctionTypeExpr(std::vector<std::unique_ptr<TypeExpr>> param_types, std::unique_ptr<TypeExpr> return_type)
        : param_types(std::move(param_types)), return_type(std::move(return_type)) {}
    std::unique_ptr<TypeExpr> clone() const override {
        std::vector<std::unique_ptr<TypeExpr>> cloned_params;
        for (const auto& param : param_types) {
            cloned_params.push_back(param->clone());
        }
        return std::make_unique<FunctionTypeExpr>(std::move(cloned_params), return_type->clone());
    }
};

struct GenericTypeExpr : TypeExpr {
    Token base_type;
    std::vector<std::unique_ptr<TypeExpr>> generic_args;
    GenericTypeExpr(Token base_type, std::vector<std::unique_ptr<TypeExpr>> generic_args)
        : base_type(std::move(base_type)), generic_args(std::move(generic_args)) {}
    std::unique_ptr<TypeExpr> clone() const override {
        std::vector<std::unique_ptr<TypeExpr>> cloned_args;
        for (const auto& arg : generic_args) {
            cloned_args.push_back(arg->clone());
        }
        return std::make_unique<GenericTypeExpr>(base_type, std::move(cloned_args));
    }
};

struct BorrowTypeExpr : TypeExpr {
    std::unique_ptr<TypeExpr> element_type;
    bool isMutable;
    BorrowTypeExpr(std::unique_ptr<TypeExpr> element_type, bool isMutable)
        : element_type(std::move(element_type)), isMutable(isMutable) {}
    std::unique_ptr<TypeExpr> clone() const override {
        return std::make_unique<BorrowTypeExpr>(element_type->clone(), isMutable);
    }
};

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
struct Stmt {
    virtual ~Stmt() = default;
    virtual std::any accept(StmtVisitor& visitor) const = 0;
    virtual std::unique_ptr<Stmt> clone() const = 0;
};

// Concrete Expression Nodes
struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitBinaryExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<BinaryExpr>(left->clone(), op, right->clone());
    }
};

struct UnaryExpr : Expr {
    Token op;
    std::unique_ptr<Expr> right;
    bool is_postfix;
    UnaryExpr(Token op, std::unique_ptr<Expr> right, bool is_postfix = false)
        : op(std::move(op)), right(std::move(right)), is_postfix(is_postfix) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitUnaryExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<UnaryExpr>(op, right->clone(), is_postfix);
    }
};

struct LiteralExpr : Expr {
    std::variant<std::nullptr_t, std::string, double, long long, bool, char> value;
    explicit LiteralExpr(std::variant<std::nullptr_t, std::string, double, long long, bool, char> value) : value(std::move(value)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitLiteralExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<LiteralExpr>(value);
    }
};

struct GroupingExpr : Expr {
    std::unique_ptr<Expr> expression;
    explicit GroupingExpr(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitGroupingExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<GroupingExpr>(expression->clone());
    }
};

struct VariableExpr : Expr {
    Token name;
    explicit VariableExpr(Token name) : name(std::move(name)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitVariableExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<VariableExpr>(name);
    }
};

struct AssignExpr : Expr {
    Token name;
    std::unique_ptr<Expr> value;
    AssignExpr(Token name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitAssignExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<AssignExpr>(name, value->clone());
    }
};

struct CallExpr : Expr {
    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<TypeExpr>> generic_args;
    std::vector<std::unique_ptr<Expr>> arguments;
    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<TypeExpr>> generic_args, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), generic_args(std::move(generic_args)), arguments(std::move(arguments)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitCallExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        std::vector<std::unique_ptr<TypeExpr>> cloned_generic_args;
        for (const auto& arg : generic_args) {
            cloned_generic_args.push_back(arg->clone());
        }
        std::vector<std::unique_ptr<Expr>> cloned_arguments;
        for (const auto& arg : arguments) {
            cloned_arguments.push_back(arg->clone());
        }
        return std::make_unique<CallExpr>(callee->clone(), paren, std::move(cloned_generic_args), std::move(cloned_arguments));
    }
};

struct LambdaExpr : Expr {
    // Simplified for now
    std::vector<Token> params;
    std::unique_ptr<Stmt> body;
    LambdaExpr(std::vector<Token> params, std::unique_ptr<Stmt> body)
        : params(std::move(params)), body(std::move(body)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitLambdaExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        // body is a Stmt, so we need to cast it.
        return std::make_unique<LambdaExpr>(params, body ? body->clone() : nullptr);
    }
};

struct GetExpr : Expr {
    std::unique_ptr<Expr> object;
    Token name;
    GetExpr(std::unique_ptr<Expr> object, Token name)
        : object(std::move(object)), name(std::move(name)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitGetExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<GetExpr>(object->clone(), name);
    }
};

struct SetExpr : Expr {
    std::unique_ptr<Expr> object;
    Token name;
    std::unique_ptr<Expr> value;
    SetExpr(std::unique_ptr<Expr> object, Token name, std::unique_ptr<Expr> value)
        : object(std::move(object)), name(std::move(name)), value(std::move(value)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitSetExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<SetExpr>(object->clone(), name, value->clone());
    }
};

struct SelfExpr : Expr {
    Token keyword;
    explicit SelfExpr(Token keyword) : keyword(std::move(keyword)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitSelfExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<SelfExpr>(keyword);
    }
};

struct BorrowExpr : Expr {
    std::unique_ptr<Expr> expression;
    bool isMutable;
    BorrowExpr(std::unique_ptr<Expr> expression, bool isMutable)
        : expression(std::move(expression)), isMutable(isMutable) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitBorrowExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<BorrowExpr>(expression->clone(), isMutable);
    }
};

struct DerefExpr : Expr {
    std::unique_ptr<Expr> expression;
    explicit DerefExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitDerefExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<DerefExpr>(expression->clone());
    }
};

struct StructLiteralExpr : Expr {
    Token name;
    std::map<std::string, std::unique_ptr<Expr>> fields;
    StructLiteralExpr(Token name, std::map<std::string, std::unique_ptr<Expr>> fields)
        : name(std::move(name)), fields(std::move(fields)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitStructLiteralExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        std::map<std::string, std::unique_ptr<Expr>> cloned_fields;
        for (const auto& field : fields) {
            cloned_fields[field.first] = field.second->clone();
        }
        return std::make_unique<StructLiteralExpr>(name, std::move(cloned_fields));
    }
};

struct ArrayLiteralExpr : Expr {
    std::vector<std::unique_ptr<Expr>> elements;
    explicit ArrayLiteralExpr(std::vector<std::unique_ptr<Expr>> elements)
        : elements(std::move(elements)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitArrayLiteralExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        std::vector<std::unique_ptr<Expr>> cloned_elements;
        for (const auto& element : elements) {
            cloned_elements.push_back(element->clone());
        }
        return std::make_unique<ArrayLiteralExpr>(std::move(cloned_elements));
    }
};

struct TypeCastExpr : Expr {
    std::unique_ptr<TypeExpr> type;
    std::unique_ptr<Expr> expression;
    TypeCastExpr(std::unique_ptr<TypeExpr> type, std::unique_ptr<Expr> expression)
        : type(std::move(type)), expression(std::move(expression)) {}
    std::any accept(ExprVisitor& visitor) const override { return visitor.visitTypeCastExpr(*this); }
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<TypeCastExpr>(type->clone(), expression->clone());
    }
};


// Concrete Statement Nodes
struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) : statements(std::move(statements)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitBlockStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        std::vector<std::unique_ptr<Stmt>> cloned_statements;
        for (const auto& stmt : statements) {
            cloned_statements.push_back(stmt->clone());
        }
        return std::make_unique<BlockStmt>(std::move(cloned_statements));
    }
};

struct ExpressionStmt : Stmt {
    std::unique_ptr<Expr> expression;
    explicit ExpressionStmt(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitExpressionStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<ExpressionStmt>(expression->clone());
    }
};

struct FunctionStmt : Stmt {
    Token name;
    std::vector<Token> generic_params;
    std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>> generic_constraints;
    std::vector<Token> params;
    std::vector<std::unique_ptr<TypeExpr>> param_types;
    std::unique_ptr<TypeExpr> return_type;
    std::unique_ptr<BlockStmt> body;
    std::unique_ptr<TypeExpr> trait_impl; // New field
    FunctionStmt(Token name, std::vector<Token> generic_params, std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>> generic_constraints, std::vector<Token> params, std::vector<std::unique_ptr<TypeExpr>> param_types, std::unique_ptr<TypeExpr> return_type, std::unique_ptr<BlockStmt> body, std::unique_ptr<TypeExpr> trait_impl = nullptr)
        : name(std::move(name)), generic_params(std::move(generic_params)), generic_constraints(std::move(generic_constraints)), params(std::move(params)), param_types(std::move(param_types)), return_type(std::move(return_type)), body(std::move(body)), trait_impl(std::move(trait_impl)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitFunctionStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        std::vector<std::unique_ptr<TypeExpr>> cloned_param_types;
        for (const auto& p_type : param_types) {
            cloned_param_types.push_back(p_type->clone());
        }

        std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>> cloned_generic_constraints;
        for (const auto& [name, constraints] : generic_constraints) {
            std::vector<std::unique_ptr<TypeExpr>> cloned_constraints;
            for (const auto& constraint : constraints) {
                cloned_constraints.push_back(constraint->clone());
            }
            cloned_generic_constraints[name] = std::move(cloned_constraints);
        }

        return std::make_unique<FunctionStmt>(name, generic_params, std::move(cloned_generic_constraints), params, std::move(cloned_param_types), return_type ? return_type->clone() : nullptr, body ? std::unique_ptr<BlockStmt>(static_cast<BlockStmt*>(body->clone().release())) : nullptr, trait_impl ? trait_impl->clone() : nullptr);
    }
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitIfStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<IfStmt>(condition->clone(), thenBranch->clone(), elseBranch ? elseBranch->clone() : nullptr);
    }
};

struct VarStmt : Stmt {
    Token name;
    std::unique_ptr<TypeExpr> type;
    std::unique_ptr<Expr> initializer;
    bool isMutable;
    VarStmt(Token name, std::unique_ptr<TypeExpr> type, std::unique_ptr<Expr> initializer, bool isMutable)
        : name(std::move(name)), type(std::move(type)), initializer(std::move(initializer)), isMutable(isMutable) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitVarStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<VarStmt>(name, type ? type->clone() : nullptr, initializer ? initializer->clone() : nullptr, isMutable);
    }
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitWhileStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<WhileStmt>(condition->clone(), body->clone());
    }
};

struct ForStmt : Stmt {
    std::unique_ptr<Stmt> initializer;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> increment;
    std::unique_ptr<Stmt> body;
    ForStmt(std::unique_ptr<Stmt> initializer, std::unique_ptr<Expr> condition, std::unique_ptr<Expr> increment, std::unique_ptr<Stmt> body)
        : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitForStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<ForStmt>(initializer ? initializer->clone() : nullptr, condition ? condition->clone() : nullptr, increment ? increment->clone() : nullptr, body->clone());
    }
};

struct ReturnStmt : Stmt {
    Token keyword;
    std::unique_ptr<Expr> value;
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitReturnStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<ReturnStmt>(keyword, value ? value->clone() : nullptr);
    }
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
    std::unique_ptr<Stmt> clone() const override {
        std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>> cloned_generic_constraints;
        for (const auto& [name, constraints] : generic_constraints) {
            std::vector<std::unique_ptr<TypeExpr>> cloned_constraints;
            for (const auto& constraint : constraints) {
                cloned_constraints.push_back(constraint->clone());
            }
            cloned_generic_constraints[name] = std::move(cloned_constraints);
        }

        std::vector<std::unique_ptr<Expr>> cloned_traits;
        for (const auto& trait : traits) {
            cloned_traits.push_back(trait->clone());
        }

        std::vector<std::unique_ptr<VarStmt>> cloned_fields;
        for (const auto& field : fields) {
            cloned_fields.push_back(std::unique_ptr<VarStmt>(static_cast<VarStmt*>(field->clone().release())));
        }

        std::vector<std::unique_ptr<FunctionStmt>> cloned_methods;
        for (const auto& method : methods) {
            cloned_methods.push_back(std::unique_ptr<FunctionStmt>(static_cast<FunctionStmt*>(method->clone().release())));
        }

        return std::make_unique<StructStmt>(name, generic_params, std::move(cloned_generic_constraints), std::move(cloned_traits), std::move(cloned_fields), std::move(cloned_methods));
    }
};

struct ImportStmt : Stmt {
    Token keyword;
    std::variant<std::string, Token> path; // string for file path, Token for module
    ImportStmt(Token keyword, std::variant<std::string, Token> path)
        : keyword(std::move(keyword)), path(std::move(path)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitImportStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<ImportStmt>(keyword, path);
    }
};

struct CaseStmt : Stmt {
    std::unique_ptr<Expr> value;
    std::unique_ptr<Stmt> body;
    CaseStmt(std::unique_ptr<Expr> value, std::unique_ptr<Stmt> body)
        : value(std::move(value)), body(std::move(body)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitCaseStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<CaseStmt>(value ? value->clone() : nullptr, body->clone());
    }
};

struct SwitchStmt : Stmt {
    std::unique_ptr<Expr> expression;
    std::vector<std::unique_ptr<CaseStmt>> cases;
    SwitchStmt(std::unique_ptr<Expr> expression, std::vector<std::unique_ptr<CaseStmt>> cases)
        : expression(std::move(expression)), cases(std::move(cases)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitSwitchStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        std::vector<std::unique_ptr<CaseStmt>> cloned_cases;
        for (const auto& c : cases) {
            cloned_cases.push_back(std::unique_ptr<CaseStmt>(static_cast<CaseStmt*>(c->clone().release())));
        }
        return std::make_unique<SwitchStmt>(expression->clone(), std::move(cloned_cases));
    }
};

struct BreakStmt : Stmt {
    Token keyword;
    explicit BreakStmt(Token keyword) : keyword(std::move(keyword)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitBreakStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<BreakStmt>(keyword);
    }
};

struct FallthroughStmt : Stmt {
    Token keyword;
    explicit FallthroughStmt(Token keyword) : keyword(std::move(keyword)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitFallthroughStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<FallthroughStmt>(keyword);
    }
};

struct EnumStmt : Stmt {
    Token name;
    std::vector<Token> members;
    EnumStmt(Token name, std::vector<Token> members)
        : name(std::move(name)), members(std::move(members)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitEnumStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        return std::make_unique<EnumStmt>(name, members);
    }
};

struct TraitStmt : Stmt {
    Token name;
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    TraitStmt(Token name, std::vector<std::unique_ptr<FunctionStmt>> methods)
        : name(std::move(name)), methods(std::move(methods)) {}
    std::any accept(StmtVisitor& visitor) const override { return visitor.visitTraitStmt(*this); }
    std::unique_ptr<Stmt> clone() const override {
        std::vector<std::unique_ptr<FunctionStmt>> cloned_methods;
        for (const auto& method : methods) {
            cloned_methods.push_back(std::unique_ptr<FunctionStmt>(static_cast<FunctionStmt*>(method->clone().release())));
        }
        return std::make_unique<TraitStmt>(name, std::move(cloned_methods));
    }
};

} // namespace chtholly

#endif // CHTHOLLY_AST_H
