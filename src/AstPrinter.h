#ifndef CHTHOLLY_AST_PRINTER_H
#define CHTHOLLY_AST_PRINTER_H

#include "AST.h"
#include <string>
#include <vector>
#include <any>

namespace chtholly {

class TypePrinter : public TypeExprVisitor {
public:
    std::string print(const TypeExpr& type);
    std::any visitBaseTypeExpr(const BaseTypeExpr& type) override;
    std::any visitGenericTypeExpr(const GenericTypeExpr& type) override;
    std::any visitArrayTypeExpr(const ArrayTypeExpr& type) override;
    std::any visitBorrowTypeExpr(const BorrowTypeExpr& type) override;
    std::any visitFunctionTypeExpr(const FunctionTypeExpr& type) override;
};

class AstPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const std::vector<std::unique_ptr<Stmt>>& statements, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums);

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
    template<typename... Args>
    std::string parenthesize(const std::string& name, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums, Args... args);
};

} // namespace chtholly

#endif // CHTHOLLY_AST_PRINTER_H
