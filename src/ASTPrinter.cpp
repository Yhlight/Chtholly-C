#include <vector>
#include <string>
#include <sstream>
#include "ASTPrinter.h"
#include "Token.h"

std::string ASTPrinter::print(const std::shared_ptr<Expr>& expr) {
    return std::any_cast<std::string>(expr->accept(*this));
}

std::any ASTPrinter::visitBinaryExpr(const std::shared_ptr<Binary>& expr) {
    return parenthesize(expr->op.lexeme, {expr->left, expr->right});
}

std::any ASTPrinter::visitGroupingExpr(const std::shared_ptr<Grouping>& expr) {
    return parenthesize("group", {expr->expression});
}

std::any ASTPrinter::visitLiteralExpr(const std::shared_ptr<Literal>& expr) {
    if (expr->value.has_value()) {
        if (expr->value.type() == typeid(std::string)) {
            return "\"" + std::any_cast<std::string>(expr->value) + "\"";
        }
        if (expr->value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(expr->value));
        }
        if (expr->value.type() == typeid(bool)) {
            return std::any_cast<bool>(expr->value) ? "true" : "false";
        }
    }
    return "nil";
}

std::any ASTPrinter::visitUnaryExpr(const std::shared_ptr<Unary>& expr) {
    return parenthesize(expr->op.lexeme, {expr->right});
}

std::any ASTPrinter::visitVariableExpr(const std::shared_ptr<Variable>& expr) {
    return expr->name.lexeme;
}

std::any ASTPrinter::visitAssignExpr(const std::shared_ptr<Assign>& expr) {
    return parenthesize("= " + expr->name.lexeme, {expr->value});
}

std::any ASTPrinter::visitLogicalExpr(const std::shared_ptr<Logical>& expr) {
    return parenthesize(expr->op.lexeme, {expr->left, expr->right});
}

std::any ASTPrinter::visitCallExpr(const std::shared_ptr<Call>& expr) {
    return parenthesize("call " + std::any_cast<std::string>(expr->callee->accept(*this)), expr->arguments);
}

std::any ASTPrinter::visitGetExpr(const std::shared_ptr<Get>& expr) {
    return parenthesize("." + expr->name.lexeme, {expr->object});
}

std::any ASTPrinter::visitSetExpr(const std::shared_ptr<Set>& expr) {
    return parenthesize("= ." + expr->name.lexeme, {expr->object, expr->value});
}


std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expr>>& exprs) {
    std::stringstream builder;

    builder << "(" << name;
    for (const auto& expr : exprs) {
        builder << " ";
        builder << std::any_cast<std::string>(expr->accept(*this));
    }
    builder << ")";

    return builder.str();
}
