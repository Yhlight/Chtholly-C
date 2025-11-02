#include "Transpiler.h"
#include <sstream>
#include <iostream>

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    out << "#include <iostream>\n";
    out << "#include <variant>\n\n";
    out << "int main() {\n";
    for (const auto& statement : statements) {
        execute(*statement);
    }
    out << "    return 0;\n";
    out << "}\n";
    return out.str();
}

void Transpiler::execute(const Stmt& stmt) {
    stmt.accept(*this);
}

std::string Transpiler::evaluate(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::any Transpiler::visitBinaryExpr(const BinaryExpr& expr) {
    return "(" + evaluate(*expr.left) + " " + expr.op.lexeme + " " + evaluate(*expr.right) + ")";
}

std::any Transpiler::visitGroupingExpr(const GroupingExpr& expr) {
    return "(" + evaluate(*expr.expression) + ")";
}

std::any Transpiler::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return "\"" + std::get<std::string>(expr.value) + "\"";
    } else if (std::holds_alternative<double>(expr.value)) {
        std::ostringstream oss;
        oss << std::get<double>(expr.value);
        return oss.str();
    } else if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    }
    return "nullptr";
}

std::any Transpiler::visitUnaryExpr(const UnaryExpr& expr) {
    return "(" + expr.op.lexeme + evaluate(*expr.right) + ")";
}

std::any Transpiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    out << "    " << evaluate(*stmt.expression) << ";\n";
    return {};
}

std::any Transpiler::visitPrintStmt(const PrintStmt& stmt) {
    out << "    std::cout << " << evaluate(*stmt.expression) << " << std::endl;\n";
    return {};
}

std::any Transpiler::visitLetStmt(const LetStmt& stmt) {
    out << "    ";
    if (!stmt.isMutable) {
        out << "const ";
    }
    out << "auto " << stmt.name.lexeme;
    if (stmt.initializer) {
        out << " = " << evaluate(*stmt.initializer);
    }
    out << ";\n";
    return {};
}

std::any Transpiler::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any Transpiler::visitAssignExpr(const AssignExpr& expr) {
    return expr.name.lexeme + " = " + evaluate(*expr.value);
}

std::any Transpiler::visitBlockStmt(const BlockStmt& stmt) {
    out << "    {\n";
    for (const auto& statement : stmt.statements) {
        execute(*statement);
    }
    out << "    }\n";
    return {};
}

std::any Transpiler::visitIfStmt(const IfStmt& stmt) {
    out << "    if (" << evaluate(*stmt.condition) << ") ";
    execute(*stmt.thenBranch);
    if (stmt.elseBranch) {
        out << " else ";
        execute(*stmt.elseBranch);
    }
    return {};
}

std::any Transpiler::visitWhileStmt(const WhileStmt& stmt) {
    out << "    while (" << evaluate(*stmt.condition) << ") ";
    execute(*stmt.body);
    return {};
}
