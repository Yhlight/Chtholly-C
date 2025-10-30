#include "Transpiler.h"
#include <sstream>
#include <stdexcept>

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    std::stringstream out;
    for (const auto& stmt : statements) {
        out << std::any_cast<std::string>(stmt->accept(*this));
    }
    return out.str();
}

std::any Transpiler::visit(const NumericLiteral& expr) {
    return expr.value.lexeme;
}

std::any Transpiler::visit(const UnaryExpr& expr) {
    std::string op = expr.op.lexeme;
    std::string right = std::any_cast<std::string>(expr.right->accept(*this));
    return "(" + op + right + ")";
}

std::any Transpiler::visit(const BinaryExpr& expr) {
    std::string left = std::any_cast<std::string>(expr.left->accept(*this));
    std::string op = expr.op.lexeme;
    std::string right = std::any_cast<std::string>(expr.right->accept(*this));
    return "(" + left + " " + op + " " + right + ")";
}

std::any Transpiler::visit(const GroupingExpr& expr) {
    return std::any_cast<std::string>(expr.expression->accept(*this));
}

std::any Transpiler::visit(const LetStmt& stmt) {
    std::stringstream out;
    out << "auto " << stmt.name.lexeme;
    if (stmt.initializer) {
        out << " = " << std::any_cast<std::string>(stmt.initializer->accept(*this));
    }
    out << ";\n";
    return out.str();
}
