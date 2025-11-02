#include "../include/SemanticAnalyzer.h"

std::vector<std::string> SemanticAnalyzer::analyze(const std::vector<std::shared_ptr<Stmt>>& statements) {
    errors.clear();
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    return errors;
}

void SemanticAnalyzer::visitExpressionStmt(std::shared_ptr<Expression> stmt) {
    stmt->expression->accept(*this);
}

void SemanticAnalyzer::visitVarStmt(std::shared_ptr<Var> stmt) {
    if (stmt->initializer) {
        stmt->initializer->accept(*this);
    }
    moved_vars[stmt->name.lexeme] = false;
}

void SemanticAnalyzer::visitBlockStmt(std::shared_ptr<Block> stmt) {
    for (const auto& statement : stmt->statements) {
        statement->accept(*this);
    }
}

void SemanticAnalyzer::visitFuncStmt(std::shared_ptr<Func> stmt) {
    for (const auto& statement : stmt->body) {
        statement->accept(*this);
    }
}

void SemanticAnalyzer::visitReturnStmt(std::shared_ptr<Return> stmt) {
    if (stmt->value) {
        stmt->value->accept(*this);
    }
}

void SemanticAnalyzer::visitStructStmt(std::shared_ptr<Struct> stmt) {
    for (const auto& method : stmt->methods) {
        method->accept(*this);
    }
}

std::any SemanticAnalyzer::visitBinaryExpr(std::shared_ptr<Binary> expr) {
    expr->left->accept(*this);
    expr->right->accept(*this);
    return nullptr;
}

std::any SemanticAnalyzer::visitGroupingExpr(std::shared_ptr<Grouping> expr) {
    expr->expression->accept(*this);
    return nullptr;
}

std::any SemanticAnalyzer::visitLiteralExpr(std::shared_ptr<Literal> expr) {
    return nullptr;
}

std::any SemanticAnalyzer::visitUnaryExpr(std::shared_ptr<Unary> expr) {
    expr->right->accept(*this);
    return nullptr;
}

std::any SemanticAnalyzer::visitVariableExpr(std::shared_ptr<Variable> expr) {
    if (moved_vars.count(expr->name.lexeme) && moved_vars[expr->name.lexeme]) {
        errors.push_back("Error: use of moved value '" + expr->name.lexeme + "'");
    }
    return nullptr;
}

std::any SemanticAnalyzer::visitAssignExpr(std::shared_ptr<Assign> expr) {
    expr->value->accept(*this);
    moved_vars[expr->name.lexeme] = false;
    return nullptr;
}

std::any SemanticAnalyzer::visitCallExpr(std::shared_ptr<Call> expr) {
    for (const auto& arg : expr->arguments) {
        arg->accept(*this);
        if (auto var = std::dynamic_pointer_cast<Variable>(arg)) {
            if (arg->type && !arg->type->is_ref) {
                moved_vars[var->name.lexeme] = true;
            }
        }
    }
    return nullptr;
}

std::any SemanticAnalyzer::visitGetExpr(std::shared_ptr<Get> expr) {
    expr->object->accept(*this);
    return nullptr;
}

std::any SemanticAnalyzer::visitInstantiationExpr(std::shared_ptr<Instantiation> expr) {
    for (const auto& value : expr->values) {
        value->accept(*this);
    }
    return nullptr;
}
