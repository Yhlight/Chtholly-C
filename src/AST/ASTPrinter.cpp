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
    if (stmt.type) {
        out << " : " << stmt.type->toString();
    }
    if (stmt.initializer) {
        out << " = " << print(*stmt.initializer);
    }
    out << ")";
    return out.str();
}

std::any ASTPrinter::visit(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any ASTPrinter::visit(const FuncStmt& stmt) {
    std::stringstream out;
    out << "(func " << stmt.name.lexeme << "(";
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        out << stmt.params[i].name.lexeme << " : " << stmt.params[i].type->toString();
        if (i < stmt.params.size() - 1) {
            out << ", ";
        }
    }
    out << ")";
    if (stmt.returnType) {
        out << " -> " << stmt.returnType->toString();
    }
    out << " " << std::any_cast<std::string>(stmt.body->accept(*this)) << ")";
    return out.str();
}

std::any ASTPrinter::visit(const BlockStmt& stmt) {
    std::stringstream out;
    out << "{ ";
    for (const auto& statement : stmt.statements) {
        out << std::any_cast<std::string>(statement->accept(*this)) << " ";
    }
    out << "}";
    return out.str();
}

std::any ASTPrinter::visit(const ReturnStmt& stmt) {
    std::stringstream out;
    out << "(return";
    if (stmt.value) {
        out << " " << print(*stmt.value);
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
