#include "AstPrinter.h"
#include <sstream>

std::string AstPrinter::print(const std::shared_ptr<Stmt>& stmt) {
    stmt->accept(*this);
    return ss.str();
}

void AstPrinter::visitExpressionStmt(const std::shared_ptr<ExpressionStmt>& stmt) {
    ss << parenthesize("expr-stmt", {stmt->expression});
}

void AstPrinter::visitPrintStmt(const std::shared_ptr<PrintStmt>& stmt) {
    ss << parenthesize("print", {stmt->expression});
}

void AstPrinter::visitVarStmt(const std::shared_ptr<VarStmt>& stmt) {
    ss << "(var " << stmt->name.lexeme;
    if (stmt->initializer) {
        ss << " = " << std::any_cast<std::string>(stmt->initializer->accept(*this));
    }
    ss << ")";
}


std::any AstPrinter::visitBinaryExpr(const std::shared_ptr<Binary>& expr) {
    return parenthesize(expr->op.lexeme, {expr->left, expr->right});
}

std::any AstPrinter::visitGroupingExpr(const std::shared_ptr<Grouping>& expr) {
    return parenthesize("group", {expr->expression});
}

std::any AstPrinter::visitLiteralExpr(const std::shared_ptr<Literal>& expr) {
    if (expr->value.has_value()) {
        if (expr->value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(expr->value));
        }
        if (expr->value.type() == typeid(std::string)) {
            return std::any_cast<std::string>(expr->value);
        }
        if (expr->value.type() == typeid(bool)) {
            return std::any_cast<bool>(expr->value) ? "true" : "false";
        }
    }
    return "nil";
}

std::any AstPrinter::visitUnaryExpr(const std::shared_ptr<Unary>& expr) {
    return parenthesize(expr->op.lexeme, {expr->right});
}

std::any AstPrinter::visitVariableExpr(const std::shared_ptr<Variable>& expr) {
    return expr->name.lexeme;
}

std::any AstPrinter::visitAssignExpr(const std::shared_ptr<Assign>& expr) {
    return parenthesize("= " + expr->name.lexeme, {expr->value});
}


std::string AstPrinter::parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expr>>& exprs) {
    std::stringstream builder;
    builder << "(" << name;
    for (const auto& expr : exprs) {
        builder << " ";
        builder << std::any_cast<std::string>(expr->accept(*this));
    }
    builder << ")";
    return builder.str();
}
