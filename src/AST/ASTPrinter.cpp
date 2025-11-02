#include "ASTPrinter.h"
#include <sstream>

std::string ASTPrinter::print(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::any ASTPrinter::visitBinaryExpr(const BinaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
}

std::any ASTPrinter::visitGroupingExpr(const GroupingExpr& expr) {
    return parenthesize("group", {expr.expression.get()});
}

std::any ASTPrinter::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return std::get<std::string>(expr.value);
    } else if (std::holds_alternative<double>(expr.value)) {
        std::ostringstream oss;
        oss << std::get<double>(expr.value);
        return oss.str();
    } else if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    }
    return "nil";
}

std::any ASTPrinter::visitUnaryExpr(const UnaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.right.get()});
}

std::any ASTPrinter::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any ASTPrinter::visitAssignExpr(const AssignExpr& expr) {
    return parenthesize("= " + expr.name.lexeme, {expr.value.get()});
}

std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
    std::stringstream builder;
    builder << "(" << name;
    for (const auto& expr : exprs) {
        builder << " ";
        builder << std::any_cast<std::string>(expr->accept(*this));
    }
    builder << ")";
    return builder.str();
}
