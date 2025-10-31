#include "ASTPrinter.h"

std::string ASTPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
    result_ = "";
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    return result_;
}

void ASTPrinter::visitBinaryExpr(const Binary& expr) {
    parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
}

void ASTPrinter::visitGroupingExpr(const Grouping& expr) {
    parenthesize("group", {expr.expression.get()});
}

void ASTPrinter::visitLiteralExpr(const Literal& expr) {
    result_ += expr.value.lexeme;
}

void ASTPrinter::visitUnaryExpr(const Unary& expr) {
    parenthesize(expr.op.lexeme, {expr.right.get()});
}

void ASTPrinter::visitVariableExpr(const Variable& expr) {
    result_ += expr.name.lexeme;
}

void ASTPrinter::visitBlockStmt(const Block& stmt) {
    result_ += "(block ";
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    result_ += ")";
}

void ASTPrinter::visitExpressionStmt(const ExpressionStmt& stmt) {
    parenthesize(";", {stmt.expression.get()});
}

void ASTPrinter::visitLetStmt(const LetStmt& stmt) {
    result_ += "(let " + stmt.name.lexeme;
    if (stmt.initializer) {
        result_ += " = ";
        stmt.initializer->accept(*this);
    }
    result_ += ")";
}

void ASTPrinter::visitPrintStmt(const PrintStmt& stmt) {
    parenthesize("print", {stmt.expression.get()});
}

void ASTPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
    result_ += "(" + name;
    for (const auto& expr : exprs) {
        result_ += " ";
        expr->accept(*this);
    }
    result_ += ")";
}
