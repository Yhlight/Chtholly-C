#include "ASTPrinter.h"

std::string ASTPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
    result_ = "";
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    return result_;
}

void ASTPrinter::visitAssignExpr(const Assign& expr) {
    parenthesize("= " + expr.name.lexeme, {expr.value.get()});
}

void ASTPrinter::visitBinaryExpr(const Binary& expr) {
    parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
}

void ASTPrinter::visitCallExpr(const Call& expr) {
    result_ += "(call ";
    expr.callee->accept(*this);
    for (const auto& arg : expr.arguments) {
        result_ += " ";
        arg->accept(*this);
    }
    result_ += ")";
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

void ASTPrinter::visitFuncStmt(const FuncStmt& stmt) {
    result_ += "(func " + stmt.name.lexeme + "(";
    for (const auto& param : stmt.params) {
        result_ += param.second->toString() + " " + param.first.lexeme + ", ";
    }
    result_ += ") ";
    if (stmt.returnType) {
        result_ += "-> " + stmt.returnType->toString() + " ";
    }
    stmt.body->accept(*this);
    result_ += ")";
}

void ASTPrinter::visitLetStmt(const LetStmt& stmt) {
    if (stmt.isMutable) {
        result_ += "(mut " + stmt.name.lexeme;
    } else {
        result_ += "(let " + stmt.name.lexeme;
    }
    if (stmt.type) {
        result_ += ": " + stmt.type->toString();
    }
    if (stmt.initializer) {
        result_ += " = ";
        stmt.initializer->accept(*this);
    }
    result_ += ")";
}

void ASTPrinter::visitPrintStmt(const PrintStmt& stmt) {
    parenthesize("print", {stmt.expression.get()});
}

void ASTPrinter::visitReturnStmt(const ReturnStmt& stmt) {
    if (stmt.value) {
        parenthesize("return", {stmt.value.get()});
    } else {
        result_ += "(return)";
    }
}

void ASTPrinter::visitIfStmt(const IfStmt& stmt) {
    result_ += "(if ";
    stmt.condition->accept(*this);
    result_ += " ";
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        result_ += " else ";
        stmt.elseBranch->accept(*this);
    }
    result_ += ")";
}

void ASTPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
    result_ += "(" + name;
    for (const auto& expr : exprs) {
        result_ += " ";
        expr->accept(*this);
    }
    result_ += ")";
}
