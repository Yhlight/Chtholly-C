#include "Transpiler.h"
#include <sstream>
#include <stdexcept>

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    std::stringstream out;
    for (const auto& stmt : statements) {
        stmt->accept(*this);
        out << result_;
        result_.clear();
    }
    return out.str();
}

std::shared_ptr<Type> Transpiler::visit(const NumericLiteral& expr) {
    result_ += expr.value.lexeme;
    return nullptr;
}

std::shared_ptr<Type> Transpiler::visit(const StringLiteral& expr) {
    result_ += expr.value.lexeme;
    return nullptr;
}

std::shared_ptr<Type> Transpiler::visit(const BooleanLiteral& expr) {
    result_ += expr.value ? "true" : "false";
    return nullptr;
}

std::shared_ptr<Type> Transpiler::visit(const UnaryExpr& expr) {
    result_ += "(" + expr.op.lexeme;
    expr.right->accept(*this);
    result_ += ")";
    return nullptr;
}

std::shared_ptr<Type> Transpiler::visit(const BinaryExpr& expr) {
    result_ += "(";
    expr.left->accept(*this);
    result_ += " " + expr.op.lexeme + " ";
    expr.right->accept(*this);
    result_ += ")";
    return nullptr;
}

std::shared_ptr<Type> Transpiler::visit(const GroupingExpr& expr) {
    expr.expression->accept(*this);
    return nullptr;
}

std::shared_ptr<Type> Transpiler::visit(const VariableExpr& expr) {
    result_ += expr.name.lexeme;
    return nullptr;
}

void Transpiler::visit(const LetStmt& stmt) {
    if (stmt.type) {
        result_ += stmt.type->toString() + " " + stmt.name.lexeme;
    } else {
        result_ += "auto " + stmt.name.lexeme;
    }

    if (stmt.initializer) {
        result_ += " = ";
        stmt.initializer->accept(*this);
    }
    result_ += ";\n";
}

void Transpiler::visit(const FuncStmt& stmt) {
    if (stmt.returnType) {
        result_ += stmt.returnType->toString();
    } else {
        result_ += "void";
    }
    result_ += " " + stmt.name.lexeme + "(";
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        result_ += stmt.params[i].type->toString() + " " + stmt.params[i].name.lexeme;
        if (i < stmt.params.size() - 1) {
            result_ += ", ";
        }
    }
    result_ += ") ";
    stmt.body->accept(*this);
}

void Transpiler::visit(const BlockStmt& stmt) {
    result_ += "{\n";
    for (const auto& statement : stmt.statements) {
        result_ += "    ";
        statement->accept(*this);
    }
    result_ += "}\n";
}

void Transpiler::visit(const ReturnStmt& stmt) {
    result_ += "return";
    if (stmt.value) {
        result_ += " ";
        stmt.value->accept(*this);
    }
    result_ += ";\n";
}
