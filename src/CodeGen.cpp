#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "CodeGen.h"
#include "AST.h"
#include "Environment.h"

CodeGen::CodeGen() {
    environment = std::make_shared<Environment>();
}

std::string CodeGen::generate(const std::vector<std::shared_ptr<Stmt>>& statements) {
    out << "#include <iostream>\n";
    out << "#include <string>\n";
    out << "\n";
    out << "int main() {\n";

    for (const auto& statement : statements) {
        execute(statement);
    }

    out << "    return 0;\n";
    out << "}\n";

    return out.str();
}

// Expression visitor methods
std::any CodeGen::visitBinaryExpr(const std::shared_ptr<Binary>& expr) {
    return evaluate(expr->left) + " " + expr->op.lexeme + " " + evaluate(expr->right);
}

std::any CodeGen::visitGroupingExpr(const std::shared_ptr<Grouping>& expr) {
    return "(" + evaluate(expr->expression) + ")";
}

std::any CodeGen::visitLiteralExpr(const std::shared_ptr<Literal>& expr) {
    return stringify(expr->value);
}

std::any CodeGen::visitUnaryExpr(const std::shared_ptr<Unary>& expr) {
    return expr->op.lexeme + evaluate(expr->right);
}

std::any CodeGen::visitVariableExpr(const std::shared_ptr<Variable>& expr) {
    return expr->name.lexeme;
}

std::any CodeGen::visitAssignExpr(const std::shared_ptr<Assign>& expr) {
    return expr->name.lexeme + " = " + evaluate(expr->value);
}

std::any CodeGen::visitLogicalExpr(const std::shared_ptr<Logical>& expr) {
    return evaluate(expr->left) + " " + expr->op.lexeme + " " + evaluate(expr->right);
}

std::any CodeGen::visitCallExpr(const std::shared_ptr<Call>& expr) {
    // Not implemented yet
    return "";
}

std::any CodeGen::visitGetExpr(const std::shared_ptr<Get>& expr) {
    // Not implemented yet
    return "";
}

std::any CodeGen::visitSetExpr(const std::shared_ptr<Set>& expr) {
    // Not implemented yet
    return "";
}

// Statement visitor methods
void CodeGen::visitExpressionStmt(const std::shared_ptr<Expression>& stmt) {
    out << "    " << evaluate(stmt->expression) << ";\n";
}

void CodeGen::visitPrintStmt(const std::shared_ptr<Print>& stmt) {
    out << "    std::cout << " << evaluate(stmt->expression) << " << std::endl;\n";
}

void CodeGen::visitVarStmt(const std::shared_ptr<Var>& stmt) {
    std::string type = (stmt->keyword.type == TokenType::MUT) ? "" : "const ";
    type += "auto"; // For now, we'll use auto for all types

    out << "    " << type << " " << stmt->name.lexeme;

    if (stmt->initializer != nullptr) {
        out << " = " << evaluate(stmt->initializer);
    }
    out << ";\n";
}

void CodeGen::visitBlockStmt(const std::shared_ptr<Block>& stmt) {
    out << "    {\n";
    for (const auto& statement : stmt->statements) {
        execute(statement);
    }
    out << "    }\n";
}

void CodeGen::visitIfStmt(const std::shared_ptr<If>& stmt) {
    // Not implemented yet
}

void CodeGen::visitWhileStmt(const std::shared_ptr<While>& stmt) {
    // Not implemented yet
}

void CodeGen::visitFunctionStmt(const std::shared_ptr<Function>& stmt) {
    // Not implemented yet
}

void CodeGen::visitReturnStmt(const std::shared_ptr<Return>& stmt) {
    // Not implemented yet
}

void CodeGen::visitStructStmt(const std::shared_ptr<Struct>& stmt) {
    // Not implemented yet
}

std::string CodeGen::stringify(const std::any& value) {
    if (value.type() == typeid(std::string)) {
        return "\"" + std::any_cast<std::string>(value) + "\"";
    }
    if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    }
    if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    }
    return "nullptr";
}

void CodeGen::execute(const std::shared_ptr<Stmt>& stmt) {
    if (stmt) {
        stmt->accept(*this);
    }
}

std::string CodeGen::evaluate(const std::shared_ptr<Expr>& expr) {
    if (expr) {
        return std::any_cast<std::string>(expr->accept(*this));
    }
    return "";
}
