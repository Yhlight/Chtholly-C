#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "AST.h"
#include <vector>
#include <string>
#include <map>

struct BorrowState {
    bool is_mutable = false;
    bool moved = false;
    int immutable_borrows = 0;
    bool mutable_borrow = false;
};

class Scope {
public:
    void declare(const std::string& name, bool is_mutable);
    bool is_declared(const std::string& name);
    bool is_mutable(const std::string& name);
    bool is_moved(const std::string& name);
    void move(const std::string& name);
    bool is_borrowed(const std::string& name);
    bool is_mutably_borrowed(const std::string& name);
    void borrow(const std::string& name, bool is_mutable);
    void unborrow(const std::string& name, bool is_mutable);

private:
    friend class SemanticAnalyzer;
    std::map<std::string, BorrowState> var_states;
};

class SemanticAnalyzer : public StmtVisitor, public ExprVisitor {
public:
    std::vector<std::string> analyze(const std::vector<std::shared_ptr<Stmt>>& statements);

private:
    // Scope management
    std::vector<Scope> scopes;
    std::vector<std::string> errors;

    void begin_scope();
    void end_scope();

    // Visitor methods
    void visitExpressionStmt(std::shared_ptr<Expression> stmt) override;
    void visitVarStmt(std::shared_ptr<Var> stmt) override;
    void visitBlockStmt(std::shared_ptr<Block> stmt) override;
    void visitFuncStmt(std::shared_ptr<Func> stmt) override;
    void visitReturnStmt(std::shared_ptr<Return> stmt) override;
    void visitStructStmt(std::shared_ptr<Struct> stmt) override;

    std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override;
    std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override;
    std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override;
    std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override;
    std::any visitVariableExpr(std::shared_ptr<Variable> expr) override;
    std::any visitAssignExpr(std::shared_ptr<Assign> expr) override;
    std::any visitCallExpr(std::shared_ptr<Call> expr) override;
    std::any visitGetExpr(std::shared_ptr<Get> expr) override;
    std::any visitInstantiationExpr(_Pragma("any") std::shared_ptr<Instantiation> expr) override;
};

#endif //CHTHOLLY_SEMANTICANALYZER_H
