#include "Transpiler.h"
#include <utility>
#include <stdexcept>

namespace chtholly {

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
    return out.str();
}

std::any Transpiler::visitBinaryExpr(const BinaryExpr& expr) {
    return std::any_cast<std::string>(expr.left->accept(*this)) + " " + expr.op.lexeme + " " + std::any_cast<std::string>(expr.right->accept(*this));
}

std::any Transpiler::visitUnaryExpr(const UnaryExpr& expr) {
    return expr.op.lexeme + std::any_cast<std::string>(expr.right->accept(*this));
}

std::any Transpiler::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::nullptr_t>(expr.value)) {
        return std::string("nullptr");
    }
    if (std::holds_alternative<std::string>(expr.value)) {
        return "\"" + std::get<std::string>(expr.value) + "\"";
    }
    if (std::holds_alternative<double>(expr.value)) {
        return std::to_string(std::get<double>(expr.value));
    }
    if (std::holds_alternative<long long>(expr.value)) {
        return std::to_string(std::get<long long>(expr.value));
    }
    if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    }
    if (std::holds_alternative<char>(expr.value)) {
        return "'" + std::string(1, std::get<char>(expr.value)) + "'";
    }
    return std::string("unknown literal");
}

std::any Transpiler::visitGroupingExpr(const GroupingExpr& expr) {
    return "(" + std::any_cast<std::string>(expr.expression->accept(*this)) + ")";
}

std::any Transpiler::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any Transpiler::visitAssignExpr(const AssignExpr& expr) {
    return expr.name.lexeme + " = " + std::any_cast<std::string>(expr.value->accept(*this));
}

// Placeholder for now
std::any Transpiler::visitCallExpr(const CallExpr& expr) { return nullptr; }
std::any Transpiler::visitLambdaExpr(const LambdaExpr& expr) { return nullptr; }
std::any Transpiler::visitGetExpr(const GetExpr& expr) { return nullptr; }
std::any Transpiler::visitSetExpr(const SetExpr& expr) { return nullptr; }
std::any Transpiler::visitSelfExpr(const SelfExpr& expr) { return nullptr; }
std::any Transpiler::visitBorrowExpr(const BorrowExpr& expr) { return nullptr; }
std::any Transpiler::visitDerefExpr(const DerefExpr& expr) { return nullptr; }

std::any Transpiler::visitBlockStmt(const BlockStmt& stmt) { return nullptr; }

std::any Transpiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    out << std::any_cast<std::string>(stmt.expression->accept(*this)) << ";\n";
    return nullptr;
}

std::any Transpiler::visitVarStmt(const VarStmt& stmt) {
    out << (stmt.isMutable ? "auto " : "const auto ") << stmt.name.lexeme;
    if (stmt.initializer) {
        out << " = " << std::any_cast<std::string>(stmt.initializer->accept(*this));
    }
    out << ";\n";
    return nullptr;
}

// Placeholder for now
std::any Transpiler::visitFunctionStmt(const FunctionStmt& stmt) { return nullptr; }
std::any Transpiler::visitIfStmt(const IfStmt& stmt) { return nullptr; }
std::any Transpiler::visitWhileStmt(const WhileStmt& stmt) { return nullptr; }
std::any Transpiler::visitReturnStmt(const ReturnStmt& stmt) { return nullptr; }
std::any Transpiler::visitStructStmt(const StructStmt& stmt) { return nullptr; }
std::any Transpiler::visitImportStmt(const ImportStmt& stmt) { return nullptr; }
std::any Transpiler::visitSwitchStmt(const SwitchStmt& stmt) { return nullptr; }
std::any Transpiler::visitCaseStmt(const CaseStmt& stmt) { return nullptr; }
std::any Transpiler::visitBreakStmt(const BreakStmt& stmt) { return nullptr; }
std::any Transpiler::visitFallthroughStmt(const FallthroughStmt& stmt) { return nullptr; }

} // namespace chtholly
