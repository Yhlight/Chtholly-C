#include "ASTPrinter.h"
#include <sstream>

std::string ASTPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
    result_.clear();
    for (const auto& stmt : statements) {
        stmt->accept(*this);
        result_ += "\n";
    }
    return result_;
}

std::string ASTPrinter::print(const Expr& expr) {
    result_.clear();
    expr.accept(*this);
    return result_;
}

std::shared_ptr<Type> ASTPrinter::visit(const NumericLiteral& expr) {
    result_ += expr.value.lexeme;
    return nullptr; // ASTPrinter doesn't care about types
}

std::shared_ptr<Type> ASTPrinter::visit(const StringLiteral& expr) {
    result_ += expr.value.lexeme;
    return nullptr;
}

std::shared_ptr<Type> ASTPrinter::visit(const BooleanLiteral& expr) {
    result_ += expr.value.lexeme;
    return nullptr;
}

std::shared_ptr<Type> ASTPrinter::visit(const UnaryExpr& expr) {
    result_ += "(" + expr.op.lexeme + " ";
    expr.right->accept(*this);
    result_ += ")";
    return nullptr;
}

std::shared_ptr<Type> ASTPrinter::visit(const BinaryExpr& expr) {
    result_ += "(" + expr.op.lexeme + " ";
    expr.left->accept(*this);
    result_ += " ";
    expr.right->accept(*this);
    result_ += ")";
    return nullptr;
}

std::shared_ptr<Type> ASTPrinter::visit(const GroupingExpr& expr) {
    result_ += "(group ";
    expr.expression->accept(*this);
    result_ += ")";
    return nullptr;
}

std::shared_ptr<Type> ASTPrinter::visit(const VariableExpr& expr) {
    result_ += expr.name.lexeme;
    return nullptr;
}

void ASTPrinter::visit(const LetStmt& stmt) {
    result_ += "(let " + stmt.name.lexeme;
    if (stmt.type) {
        result_ += " : " + stmt.type->toString();
    }
    if (stmt.initializer) {
        result_ += " = ";
        stmt.initializer->accept(*this);
    }
    result_ += ")";
}

void ASTPrinter::visit(const FuncStmt& stmt) {
    result_ += "(func " + stmt.name.lexeme + "(";
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        result_ += stmt.params[i].name.lexeme + " : " + stmt.params[i].type->toString();
        if (i < stmt.params.size() - 1) {
            result_ += ", ";
        }
    }
    result_ += ")";
    if (stmt.returnType) {
        result_ += " -> " + stmt.returnType->toString();
    }
    result_ += " ";
    stmt.body->accept(*this);
    result_ += ")";
}

void ASTPrinter::visit(const BlockStmt& stmt) {
    result_ += "{ ";
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
        result_ += " ";
    }
    result_ += "}";
}

void ASTPrinter::visit(const ReturnStmt& stmt) {
    result_ += "(return";
    if (stmt.value) {
        result_ += " ";
        stmt.value->accept(*this);
    }
    result_ += ")";
}
