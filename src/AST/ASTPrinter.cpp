#include "ASTPrinter.h"
#include <sstream>

std::string ASTPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
    std::stringstream out;
    for (const auto& stmt : statements) {
        out << std::any_cast<std::string>(stmt->accept(*this)) << std::endl;
    }
    return out.str();
}

std::string ASTPrinter::print(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::any ASTPrinter::visit(const NumericLiteral& expr) {
    return expr.value.lexeme;
}

std::any ASTPrinter::visit(const UnaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.right.get()});
}

std::any ASTPrinter::visit(const BinaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
}

std::any ASTPrinter::visit(const GroupingExpr& expr) {
    return parenthesize("group", {expr.expression.get()});
}

std::any ASTPrinter::visit(const LetStmt& stmt) {
    std::stringstream out;
    out << "(let " << stmt.name.lexeme;
    if (stmt.initializer) {
        out << " = " << print(*stmt.initializer);
    }
    out << ")";
    return out.str();
}

std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
    std::stringstream out;
    out << "(" << name;
    for (const auto& expr : exprs) {
        out << " " << std::any_cast<std::string>(expr->accept(*this));
    }
    out << ")";
    return out.str();
}
