#include "Transpiler.h"

namespace chtholly {

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        transpile(*stmt);
    }
    return out.str();
}

std::any Transpiler::visitBinaryExpr(BinaryExpr& expr) {
    return "(" + transpile(*expr.left) + " " + expr.op.lexeme + " " + transpile(*expr.right) + ")";
}

std::any Transpiler::visitUnaryExpr(UnaryExpr& expr) {
    return "(" + expr.op.lexeme + transpile(*expr.right) + ")";
}

std::any Transpiler::visitLiteralExpr(LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return "\"" + std::get<std::string>(expr.value) + "\"";
    } else if (std::holds_alternative<double>(expr.value)) {
        return std::to_string(std::get<double>(expr.value));
    } else if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    }
    return "nullptr";
}

std::any Transpiler::visitVariableExpr(VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any Transpiler::visitExpressionStmt(ExpressionStmt& stmt) {
    out << transpile(*stmt.expression) << ";" << std::endl;
    return {};
}

std::any Transpiler::visitBlockStmt(BlockStmt& stmt) {
    out << "{" << std::endl;
    for (const auto& statement : stmt.statements) {
        transpile(*statement);
    }
    out << "}" << std::endl;
    return {};
}

std::any Transpiler::visitVarStmt(VarStmt& stmt) {
    if (stmt.type) {
        out << transpile(*stmt.type) << " " << stmt.name.lexeme;
    } else {
        if (stmt.keyword.type == TokenType::LET) {
            out << "const auto " << stmt.name.lexeme;
        } else {
            out << "auto " << stmt.name.lexeme;
        }
    }

    if (stmt.initializer) {
        out << " = " << transpile(*stmt.initializer);
    }
    out << ";" << std::endl;
    return {};
}

std::any Transpiler::visitBaseTypeExpr(BaseTypeExpr& expr) {
    return expr.name.lexeme;
}

std::string Transpiler::transpile(Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::string Transpiler::transpile(TypeExpr& type) {
    return std::any_cast<std::string>(type.accept(*this));
}

void Transpiler::transpile(Stmt& stmt) {
    stmt.accept(*this);
}

} // namespace chtholly
