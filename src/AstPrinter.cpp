#include "../include/AstPrinter.h"
#include <sstream>
#include <iostream>

std::string AstPrinter::print(const std::vector<std::shared_ptr<Stmt>>& statements) {
    result.clear();
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    return result;
}

std::string AstPrinter::print(const std::shared_ptr<Expr>& expr) {
    return std::any_cast<std::string>(expr->accept(*this));
}

std::string AstPrinter::print(const std::shared_ptr<Stmt>& stmt) {
    result.clear();
    stmt->accept(*this);
    return result;
}

std::any AstPrinter::visitBinaryExpr(std::shared_ptr<Binary> expr) {
    return parenthesize(expr->op.lexeme, {expr->left, expr->right});
}

std::any AstPrinter::visitGroupingExpr(std::shared_ptr<Grouping> expr) {
    return parenthesize("group", {expr->expression});
}

std::any AstPrinter::visitLiteralExpr(std::shared_ptr<Literal> expr) {
    if (expr->value.type() == typeid(nullptr_t)) return "nil";
    if (expr->value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(expr->value);
    }
    if (expr->value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(expr->value));
    }
    if (expr->value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(expr->value));
    }
    if (expr->value.type() == typeid(bool)) {
        return std::any_cast<bool>(expr->value) ? "true" : "false";
    }
    return "unknown literal";
}

std::any AstPrinter::visitUnaryExpr(std::shared_ptr<Unary> expr) {
    return parenthesize(expr->op.lexeme, {expr->right});
}

std::any AstPrinter::visitVariableExpr(std::shared_ptr<Variable> expr) {
    return expr->name.lexeme;
}

std::any AstPrinter::visitAssignExpr(std::shared_ptr<Assign> expr) {
    return parenthesize("=", {std::make_shared<Variable>(expr->name), expr->value});
}


void AstPrinter::visitExpressionStmt(std::shared_ptr<Expression> stmt) {
    result.append(print(stmt->expression)).append(";\n");
}

void AstPrinter::visitVarStmt(std::shared_ptr<Var> stmt) {
    std::string out;
    out.append(stmt->isMutable ? "mut " : "let ");
    out.append(stmt->name.lexeme);
    if (stmt->initializer) {
        out.append(" = ").append(print(stmt->initializer));
    }
    out.append(";\n");
    result.append(out);
}

void AstPrinter::visitBlockStmt(std::shared_ptr<Block> stmt) {
    result.append("{\n");
    for (const auto& statement : stmt->statements) {
        statement->accept(*this);
    }
    result.append("}\n");
}

std::string AstPrinter::parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expr>>& exprs) {
    std::stringstream ss;
    ss << "(" << name;
    for (const auto& expr : exprs) {
        ss << " " << std::any_cast<std::string>(expr->accept(*this));
    }
    ss << ")";
    return ss.str();
}
