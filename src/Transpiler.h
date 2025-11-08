#ifndef CHTHOLLY_TRANSPILER_H
#define CHTHOLLY_TRANSPILER_H

#include "AST.h"
#include "Result.h"
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
    Transpiler(std::set<std::string>* transpiled_files);
    ~Transpiler();
    std::string transpile(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visitBinaryExpr(const BinaryExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitUnaryExpr(const UnaryExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitLiteralExpr(const LiteralExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitGroupingExpr(const GroupingExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitVariableExpr(const VariableExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitAssignExpr(const AssignExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitCallExpr(const CallExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitLambdaExpr(const LambdaExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitGetExpr(const GetExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitSetExpr(const SetExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitSelfExpr(const SelfExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitBorrowExpr(const BorrowExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitDerefExpr(const DerefExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitStructLiteralExpr(const StructLiteralExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitArrayLiteralExpr(const ArrayLiteralExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitTypeCastExpr(const TypeCastExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;

    std::any visitBlockStmt(const BlockStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    void visitBlockStmt(const BlockStmt& stmt, bool create_scope, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums);
    std::any visitExpressionStmt(const ExpressionStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitFunctionStmt(const FunctionStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitIfStmt(const IfStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitVarStmt(const VarStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitWhileStmt(const WhileStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitForStmt(const ForStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitReturnStmt(const ReturnStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitStructStmt(const StructStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitImportStmt(const ImportStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitSwitchStmt(const SwitchStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitCaseStmt(const CaseStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitBreakStmt(const BreakStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitFallthroughStmt(const FallthroughStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitEnumStmt(const EnumStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;
    std::any visitTraitStmt(const TraitStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) override;

private:
    void transpile_generic_constraints(const std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>>& constraints, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums);
    std::string transpileType(const TypeExpr& type, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums);
    std::string getTypeString(const TypeInfo& type);
    TypeInfo typeExprToTypeInfo(const TypeExpr* type, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums);
    bool has_trait(const std::string& struct_name, const std::string& module_name, const std::string& trait_name, std::map<std::string, const StructStmt*>& structs);
    TypeInfo get_type(const Expr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums);
    std::any handleMetaFunction(const CallExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums);
    std::any handleReflectFunction(const CallExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums);
    std::any handleUtilFunction(const CallExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums);
    void enterScope();
    void exitScope();
    void define(const std::string& name, const TypeInfo& type);
    TypeInfo lookup(const std::string& name);

    std::stringstream out;
    std::set<std::string> imported_modules;
    std::vector<std::map<std::string, TypeInfo>> scopes;
    std::map<std::string, const TraitStmt*> traits;
    bool is_in_switch = false;
    bool is_in_method = false;
    bool input_used = false;
    bool reflect_used = false;
    bool vector_used = false;
    bool array_used = false;
    bool optional_used = false;
    bool result_used = false;
    TypeInfo contextual_type;
    std::set<std::string>* transpiled_files;
    bool owns_transpiled_files = false;
};

} // namespace chtholly

#endif // CHTHOLLY_TRANSPILER_H
