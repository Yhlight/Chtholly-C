#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <map>
#include <set>

#include "TypeInfo.h"

// Forward declaration
class Transpiler;

inline std::string op_to_trait(const std::string& op) {
    if (op == "+") return "add";
    if (op == "-") return "sub";
    if (op == "*") return "mul";
    if (op == "/") return "div";
    if (op == "%") return "mod";
    if (op == "==") return "equal";
    if (op == "!=") return "not_equal";
    if (op == "<") return "less";
    if (op == "<=") return "less_equal";
    if (op == ">") return "greater";
    if (op == ">=") return "greater_equal";
    if (op == "&&") return "and";
    if (op == "||") return "or";
    return "";
}

class Transpiler : public ExprVisitor, public StmtVisitor {
public:
    std::string transpile(const std::vector<std::unique_ptr<Stmt>>& statements, bool is_module = false);

private:
    std::string evaluate(const Expr& expr);
    void execute(const Stmt& stmt);
    TypeInfo get_type(const Expr& expr);

    std::stringstream out;
    std::map<std::string, const ImplStmt*> impls;
    std::map<std::string, const StructStmt*> structs;
    std::set<std::string> transpiled_files;
    std::vector<std::map<std::string, TypeInfo>> symbol_table;
    std::set<std::string> imported_std_modules;
    bool is_in_method = false;

    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;
    std::any visitAssignExpr(const AssignExpr& expr) override;
    std::any visitCallExpr(const CallExpr& expr) override;
    std::any visitGetExpr(const GetExpr& expr) override;
    std::any visitSetExpr(const SetExpr& expr) override;
    std::any visitBorrowExpr(const BorrowExpr& expr) override;
    std::any visitLambdaExpr(const LambdaExpr& expr) override;

    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitLetStmt(const LetStmt& stmt) override;
    std::any visitBlockStmt(const BlockStmt& stmt) override;
    std::any visitIfStmt(const IfStmt& stmt) override;
    std::any visitWhileStmt(const WhileStmt& stmt) override;
    std::any visitFunctionStmt(const FunctionStmt& stmt) override;
    std::any visitReturnStmt(const ReturnStmt& stmt) override;
    std::any visitStructStmt(const StructStmt& stmt) override;
    std::any visitTraitStmt(const TraitStmt& stmt) override;
    std::any visitImplStmt(const ImplStmt& stmt) override;
    std::any visitImportStmt(const ImportStmt& stmt) override;
};
