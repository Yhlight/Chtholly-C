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

    std::any visitBooleanLiteralExpr(BooleanLiteralExpr& expr) override
    {
        return expr.value.text;
    }

    std::any visitVariableExpr(VariableExpr& expr) override
    {
        return expr.name.text;
    }

    std::any visitVarDeclStmt(VarDeclStmt& stmt) override
    {
        std::stringstream out;
        out << "(var " << stmt.name.text;
        if (stmt.type.type != TokenType::Unknown) {
            out << ": " << stmt.type.text;
        }
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

    std::any visitWhileStmt(WhileStmt& stmt) override {
        std::stringstream out;
        out << "(while " << std::any_cast<std::string>(stmt.condition->accept(*this)) << " ";
        out << std::any_cast<std::string>(stmt.body->accept(*this)) << ")\n";
        return out.str();
    }

    std::any visitSwitchStmt(SwitchStmt& stmt) override {
        std::stringstream out;
        out << "(switch " << std::any_cast<std::string>(stmt.expression->accept(*this)) << " { ";
        for (const auto& c : stmt.cases) {
            out << std::any_cast<std::string>(c->accept(*this)) << " ";
        }
        out << "})";
        return out.str();
    }

    std::any visitCaseStmt(CaseStmt& stmt) override {
        std::stringstream out;
        out << "(case " << std::any_cast<std::string>(stmt.expression->accept(*this)) << ": ";
        out << std::any_cast<std::string>(stmt.body->accept(*this)) << ")";
        return out.str();
    }

    std::any visitBreakStmt(BreakStmt& stmt) override {
        return std::string("(break;)\n");
    }

    std::any visitFallthroughStmt(FallthroughStmt& stmt) override {
        return std::string("(fallthrough;)\n");
    }

    std::any visitFunctionStmt(FunctionStmt& stmt) override {
        std::stringstream out;
        out << "(func " << stmt.name.text << "(";
        for (size_t i = 0; i < stmt.params.size(); ++i) {
            out << stmt.params[i].name.text << ": " << stmt.params[i].type.text;
            if (i < stmt.params.size() - 1) {
                out << ", ";
            }
        }
        out << ")";
        if (stmt.returnType.type != TokenType::Unknown) {
            out << " -> " << stmt.returnType.text;
        }
        out << " ";
        if (stmt.body) {
            out << std::any_cast<std::string>(stmt.body->accept(*this));
        }
        out << ")";
        return out.str();
    }

    std::any visitStructStmt(StructStmt& stmt) override {
        std::stringstream out;
        out << "(struct " << stmt.name.text << " { ";
        for (const auto& field : stmt.fields) {
            out << std::any_cast<std::string>(field->accept(*this)) << " ";
        }
        for (const auto& method : stmt.methods) {
            out << std::any_cast<std::string>(method->accept(*this)) << " ";
        }
        out << "})";
        return out.str();
    }
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_PRINTER_H
