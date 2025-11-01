#include "Transpiler.h"
#include <sstream>
#include <stdexcept>

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    std::stringstream out;
    out << "#include <iostream>\n";
    out << "#include <string>\n";
    out << "#include <variant>\n\n";
    out << "int main() {\n";

    for (const auto& statement : statements) {
        if (statement) {
            out << "    " << transpile(*statement) << "\n";
        }
    }

    out << "    return 0;\n";
    out << "}\n";
    return out.str();
}


std::string Transpiler::transpile(const Stmt& stmt) {
    return std::any_cast<std::string>(stmt.accept(*this));
}

std::string Transpiler::transpile(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

// Expression visitor methods
std::any Transpiler::visitAssignExpr(const AssignExpr& expr) {
    return expr.name.lexeme + " = " + transpile(*expr.value);
}

std::any Transpiler::visitBinaryExpr(const BinaryExpr& expr) {
    return "(" + transpile(*expr.left) + " " + expr.op.lexeme + " " + transpile(*expr.right) + ")";
}

std::any Transpiler::visitGroupingExpr(const GroupingExpr& expr) {
    return "(" + transpile(*expr.expression) + ")";
}

std::any Transpiler::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return "std::string(\"" + std::get<std::string>(expr.value) + "\")";
    }
    if (std::holds_alternative<double>(expr.value)) {
        return std::to_string(std::get<double>(expr.value));
    }
    if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    }
    return "nullptr"; // Representing nil
}

std::any Transpiler::visitUnaryExpr(const UnaryExpr& expr) {
    return "(" + expr.op.lexeme + transpile(*expr.right) + ")";
}

std::any Transpiler::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

// Statement visitor methods
std::any Transpiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    return transpile(*stmt.expression) + ";";
}

std::any Transpiler::visitPrintStmt(const PrintStmt& stmt) {
    return "std::cout << (" + transpile(*stmt.expression) + ") << std::endl;";
}

std::any Transpiler::visitLetStmt(const LetStmt& stmt) {
    std::string value = "auto";
    if (stmt.initializer) {
        value += " " + stmt.name.lexeme + " = " + transpile(*stmt.initializer) + ";";
    } else {
        value += " " + stmt.name.lexeme + ";";
    }
    return value;
}
