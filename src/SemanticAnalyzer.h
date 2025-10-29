#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "AST/AST.h"
#include "SymbolTable.h"
#include <vector>
#include <memory>
#include <any>

namespace Chtholly {

class SemanticAnalyzer : public ExprVisitor, public StmtVisitor {
public:
    SemanticAnalyzer();

    void analyze(const std::vector<std::unique_ptr<Stmt>>& statements);
    bool hadError = false;

private:
    std::any visitBlockStmt(BlockStmt& stmt) override;
    std::any visitVarDeclStmt(VarDeclStmt& stmt) override;
    // ... other visit methods will be added here

    std::any visitAssignExpr(AssignExpr& expr) override;
    std::any visitBinaryExpr(BinaryExpr& expr) override;
    std::any visitGroupingExpr(GroupingExpr& expr) override;
    std::any visitNumberLiteralExpr(NumberLiteralExpr& expr) override;
    std::any visitBooleanLiteralExpr(BooleanLiteralExpr& expr) override;
    std::any visitUnaryExpr(UnaryExpr& expr) override;
    std::any visitVariableExpr(VariableExpr& expr) override;
    std::any visitExprStmt(ExprStmt& stmt) override;
    std::any visitIfStmt(IfStmt& stmt) override;
    std::any visitWhileStmt(WhileStmt& stmt) override;
    std::any visitSwitchStmt(SwitchStmt& stmt) override;
    std::any visitCaseStmt(CaseStmt& stmt) override;
    std::any visitBreakStmt(BreakStmt& stmt) override;
    std::any visitFallthroughStmt(FallthroughStmt& stmt) override;
    std::any visitFunctionStmt(FunctionStmt& stmt) override;
    std::any visitStructStmt(StructStmt& stmt) override;


    SymbolTable symbolTable;
};

} // namespace Chtholly

#endif // CHTHOLLY_SEMANTICANALYZER_H
