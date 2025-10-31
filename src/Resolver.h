#ifndef CHTHOLLY_RESOLVER_H
#define CHTHOLLY_RESOLVER_H

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "Type.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

enum class OwnershipState {
    Owned,
    Moved,
    BorrowedRead,
    BorrowedWrite
};

struct Symbol {
    std::shared_ptr<Type> type;
    bool isMutable;
    OwnershipState state;
    int borrowCount = 0;
};

class Resolver : public ExprVisitor, public StmtVisitor {
public:
    void resolve(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::shared_ptr<Type> visit(const NumericLiteral& expr) override;
    std::shared_ptr<Type> visit(const StringLiteral& expr) override;
    std::shared_ptr<Type> visit(const UnaryExpr& expr) override;
    std::shared_ptr<Type> visit(const BinaryExpr& expr) override;
    std::shared_ptr<Type> visit(const GroupingExpr& expr) override;
    std::shared_ptr<Type> visit(const VariableExpr& expr) override;
    std::shared_ptr<Type> visit(const BorrowExpr& expr) override;
    std::shared_ptr<Type> visit(const CallExpr& expr) override;
    std::shared_ptr<Type> visit(const AssignExpr& expr) override;


    void visit(const LetStmt& stmt) override;
    void visit(const FuncStmt& stmt) override;
    void visit(const BlockStmt& stmt) override;
    void visit(const ReturnStmt& stmt) override;
    void visit(const ExprStmt& stmt) override;

private:
    void beginScope();
    void endScope();
    void declare(const Token& name, std::shared_ptr<Type> type, bool isMutable);
    void define(const Token& name);
    void resolve(const Stmt& stmt);
    std::shared_ptr<Type> resolve(const Expr& expr);
    Symbol* findSymbol(const Token& name);


    std::vector<std::unordered_map<std::string, Symbol>> scopes_;
    std::vector<Symbol*> borrowedSymbols_;
    std::shared_ptr<Type> currentFunction_ = nullptr;
};

#endif // CHTHOLLY_RESOLVER_H
