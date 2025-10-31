#include "Transpiler.h"

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    out_ << "#include <iostream>\n";
    out_ << "#include <string>\n\n";
    out_ << "int main() {\n";

    for (const auto& statement : statements) {
        statement->accept(*this);
    }

    out_ << "    return 0;\n";
    out_ << "}\n";
    return out_.str();
}

void Transpiler::visitBinaryExpr(const Binary& expr) {
    out_ << "(";
    expr.left->accept(*this);
    out_ << " " << expr.op.lexeme << " ";
    expr.right->accept(*this);
    out_ << ")";
}

void Transpiler::visitGroupingExpr(const Grouping& expr) {
    out_ << "(";
    expr.expression->accept(*this);
    out_ << ")";
}

void Transpiler::visitLiteralExpr(const Literal& expr) {
    out_ << expr.value.lexeme;
}

void Transpiler::visitUnaryExpr(const Unary& expr) {
    out_ << expr.op.lexeme;
    expr.right->accept(*this);
}

void Transpiler::visitVariableExpr(const Variable& expr) {
    out_ << expr.name.lexeme;
}

void Transpiler::visitBlockStmt(const Block& stmt) {
    out_ << "    {\n";
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    out_ << "    }\n";
}

void Transpiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    out_ << "    ";
    stmt.expression->accept(*this);
    out_ << ";\n";
}

void Transpiler::visitLetStmt(const LetStmt& stmt) {
    out_ << "    const auto " << stmt.name.lexeme;
    if (stmt.initializer) {
        out_ << " = ";
        stmt.initializer->accept(*this);
    }
    out_ << ";\n";
}

void Transpiler::visitPrintStmt(const PrintStmt& stmt) {
    out_ << "    std::cout << ";
    stmt.expression->accept(*this);
    out_ << " << std::endl;\n";
}
