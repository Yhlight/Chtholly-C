#ifndef CHTHOLLY_TRANSPILER_H
#define CHTHOLLY_TRANSPILER_H

#include "AST.h"
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <vector>
#include <map>

namespace chtholly {

class Transpiler : public ExprVisitor, public StmtVisitor {
public:
    Transpiler();
    std::string transpile(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visitBinaryExpr(const BinaryExpr& expr) override;
    std::any visitUnaryExpr(const UnaryExpr& expr) override;
    std::any visitLiteralExpr(const LiteralExpr& expr) override;
    std::any visitGroupingExpr(const GroupingExpr& expr) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;
    std::any visitAssignExpr(const AssignExpr& expr) override;
    std::any visitCallExpr(const CallExpr& expr) override;
    std::any visitLambdaExpr(const LambdaExpr& expr) override;
    std::any visitGetExpr(const GetExpr& expr) override;
    std::any visitSetExpr(const SetExpr& expr) override;
    std::any visitSelfExpr(const SelfExpr& expr) override;
    std::any visitBorrowExpr(const BorrowExpr& expr) override;
    std::any visitDerefExpr(const DerefExpr& expr) override;
    std::any visitStructLiteralExpr(const StructLiteralExpr& expr) override;
    std::any visitArrayLiteralExpr(const ArrayLiteralExpr& expr) override;

    std::any visitBlockStmt(const BlockStmt& stmt) override;
    void visitBlockStmt(const BlockStmt& stmt, bool create_scope);
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override;
    std::any visitFunctionStmt(const FunctionStmt& stmt) override;
    std::any visitIfStmt(const IfStmt& stmt) override;
    std::any visitVarStmt(const VarStmt& stmt) override;
    std::any visitWhileStmt(const WhileStmt& stmt) override;
    std::any visitForStmt(const ForStmt& stmt) override;
    std::any visitReturnStmt(const ReturnStmt& stmt) override;
    std::any visitStructStmt(const StructStmt& stmt) override;
    std::any visitImportStmt(const ImportStmt& stmt) override;
    std::any visitSwitchStmt(const SwitchStmt& stmt) override;
    std::any visitCaseStmt(const CaseStmt& stmt) override;
    std::any visitBreakStmt(const BreakStmt& stmt) override;
    std::any visitFallthroughStmt(const FallthroughStmt& stmt) override;
    std::any visitEnumStmt(const EnumStmt& stmt) override;

private:
    std::string transpileType(const TypeExpr& type);
    TypeInfo typeExprToTypeInfo(const TypeExpr* type);
    bool has_trait(const std::string& struct_name, const std::string& module_name, const std::string& trait_name);
    TypeInfo get_type(const Expr& expr);
    std::any handleMetaFunction(const CallExpr& expr);
    std::any handleReflectFunction(const CallExpr& expr);
    std::any handleUtilFunction(const CallExpr& expr);
    void enterScope();
    void exitScope();
    void define(const std::string& name, const TypeInfo& type);
    TypeInfo lookup(const std::string& name);

    std::stringstream out;
    std::set<std::string> imported_modules;
    std::vector<std::map<std::string, TypeInfo>> scopes;
    std::map<std::string, const StructStmt*> structs;
    std::map<std::string, const EnumStmt*> enums;
    bool is_in_switch = false;
    bool is_in_method = false;
    bool input_used = false;
    bool reflect_used = false;
    bool vector_used = false;
    bool array_used = false;
};

} // namespace chtholly

#endif // CHTHOLLY_TRANSPILER_H
