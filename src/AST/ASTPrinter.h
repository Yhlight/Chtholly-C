#ifndef CHTHOLLY_AST_PRINTER_H
#define CHTHOLLY_AST_PRINTER_H

#include "AST.h"
#include <sstream>

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
            out << std::any_cast<std::string>(stmt->accept(*this));
        }
        return out.str();
    }

    std::any visitNumberLiteralExpr(NumberLiteralExpr& expr) override
    {
        return expr.number.text;
    }

    std::any visitVarDeclStmt(VarDeclStmt& stmt) override
    {
        std::stringstream out;
        out << "(var " << stmt.name.text;
        if (stmt.initializer)
        {
            out << " = " << std::any_cast<std::string>(stmt.initializer->accept(*this));
        }
        out << ";)\\n";
        return out.str();
    }

    std::any visitBlockStmt(BlockStmt& stmt) override
    {
        std::stringstream out;
        out << "{ \\n";
        for (const auto& s : stmt.statements)
        {
            out << std::any_cast<std::string>(s->accept(*this));
        }
        out << "}\\n";
        return out.str();
    }
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_PRINTER_H
