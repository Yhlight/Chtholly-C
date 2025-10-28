#ifndef CHTHOLLY_AST_PRINTER_H
#define CHTHOLLY_AST_PRINTER_H

#include "AST.h"
#include <sstream>
#include <vector>
#include <memory>
#include <any>

namespace Chtholly
{

class ASTPrinter : public ExprVisitor, public StmtVisitor
{
public:
    std::string print(const std::vector<std::unique_ptr<Stmt>>& statements)
    {
        std::stringstream out;
        for (const auto& stmt : statements)
        {
            if (stmt) {
                out << std::any_cast<std::string>(stmt->accept(*this));
            }
        }
        return out.str();
    }

private:
    template<typename... Args>
    std::string parenthesize(const std::string& name, Args... args)
    {
        std::stringstream out;
        out << "(" << name;
        ((out << " " << std::any_cast<std::string>(args->accept(*this))), ...);
        out << ")";
        return out.str();
    }

    // --- Visitor implementations ---

    std::any visitAssignExpr(AssignExpr& expr) override
    {
        return parenthesize(expr.name.text + " =", expr.value.get());
    }

    std::any visitBinaryExpr(BinaryExpr& expr) override
    {
        return parenthesize(expr.op.text, expr.left.get(), expr.right.get());
    }

    std::any visitGroupingExpr(GroupingExpr& expr) override
    {
        return parenthesize("group", expr.expression.get());
    }

    std::any visitNumberLiteralExpr(NumberLiteralExpr& expr) override
    {
        return expr.number.text;
    }

    std::any visitUnaryExpr(UnaryExpr& expr) override
    {
        return parenthesize(expr.op.text, expr.right.get());
    }

    std::any visitVariableExpr(VariableExpr& expr) override
    {
        return expr.name.text;
    }

    std::any visitVarDeclStmt(VarDeclStmt& stmt) override
    {
        std::stringstream out;
        out << "(var " << stmt.name.text;
        if (stmt.initializer)
        {
            out << " = " << std::any_cast<std::string>(stmt.initializer->accept(*this));
        }
        out << ";)\n";
        return out.str();
    }

    std::any visitBlockStmt(BlockStmt& stmt) override
    {
        std::stringstream out;
        out << "{ \n";
        for (const auto& s : stmt.statements)
        {
            out << std::any_cast<std::string>(s->accept(*this));
        }
        out << "}";
        return out.str();
    }

    std::any visitExprStmt(ExprStmt& stmt) override {
        std::stringstream out;
        out << "(expr " << std::any_cast<std::string>(stmt.expression->accept(*this)) << ")\n";
        return out.str();
    }

    std::any visitIfStmt(IfStmt& stmt) override {
        std::stringstream out;
        out << "(if " << std::any_cast<std::string>(stmt.condition->accept(*this)) << " ";
        out << std::any_cast<std::string>(stmt.thenBranch->accept(*this));
        if (stmt.elseBranch) {
            out << " else ";
            out << std::any_cast<std::string>(stmt.elseBranch->accept(*this));
        }
        out << ")\n";
        return out.str();
    }
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_PRINTER_H
