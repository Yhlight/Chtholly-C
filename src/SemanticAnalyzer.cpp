#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "SemanticAnalyzer.h"
#include "AST.h"
#include "Environment.h"

SemanticAnalyzer::SemanticAnalyzer() {
    environment = std::make_shared<Environment>();
}

void SemanticAnalyzer::analyze(const std::vector<std::shared_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        check(statement);
    }
}

const std::vector<std::string>& SemanticAnalyzer::getErrors() const {
    return errors;
}

// Expression visitor methods
std::any SemanticAnalyzer::visitBinaryExpr(const std::shared_ptr<Binary>& expr) {
    evaluate(expr->left);
    evaluate(expr->right);
    return nullptr;
}

std::any SemanticAnalyzer::visitGroupingExpr(const std::shared_ptr<Grouping>& expr) {
    return evaluate(expr->expression);
}

std::any SemanticAnalyzer::visitLiteralExpr(const std::shared_ptr<Literal>& expr) {
    return expr->value;
}

std::any SemanticAnalyzer::visitUnaryExpr(const std::shared_ptr<Unary>& expr) {
    evaluate(expr->right);
    return nullptr;
}

std::any SemanticAnalyzer::visitVariableExpr(const std::shared_ptr<Variable>& expr) {
    try {
        return environment->get(expr->name);
    } catch (const std::runtime_error& e) {
        errors.push_back(e.what());
        return nullptr;
    }
}

std::any SemanticAnalyzer::visitAssignExpr(const std::shared_ptr<Assign>& expr) {
    evaluate(expr->value);
    try {
        environment->assign(expr->name, evaluate(expr->value));
    } catch (const std::runtime_error& e) {
        errors.push_back(e.what());
    }
    return nullptr;
}

std::any SemanticAnalyzer::visitLogicalExpr(const std::shared_ptr<Logical>& expr) {
    evaluate(expr->left);
    evaluate(expr->right);
    return nullptr;
}

std::any SemanticAnalyzer::visitCallExpr(const std::shared_ptr<Call>& expr) {
    evaluate(expr->callee);
    for (const auto& arg : expr->arguments) {
        evaluate(arg);
    }
    return nullptr;
}

std::any SemanticAnalyzer::visitGetExpr(const std::shared_ptr<Get>& expr) {
    evaluate(expr->object);
    return nullptr;
}

std::any SemanticAnalyzer::visitSetExpr(const std::shared_ptr<Set>& expr) {
    evaluate(expr->value);
    evaluate(expr->object);
    return nullptr;
}


// Statement visitor methods
void SemanticAnalyzer::visitExpressionStmt(const std::shared_ptr<Expression>& stmt) {
    evaluate(stmt->expression);
}

void SemanticAnalyzer::visitPrintStmt(const std::shared_ptr<Print>& stmt) {
    evaluate(stmt->expression);
}

void SemanticAnalyzer::visitVarStmt(const std::shared_ptr<Var>& stmt) {
    std::any value = nullptr;
    if (stmt->initializer != nullptr) {
        value = evaluate(stmt->initializer);
    }

    bool isMutable = stmt->keyword.type == TokenType::MUT;
    environment->define(stmt->name.lexeme, value, isMutable);
}

void SemanticAnalyzer::visitBlockStmt(const std::shared_ptr<Block>& stmt) {
    executeBlock(stmt->statements, std::make_shared<Environment>(environment));
}

void SemanticAnalyzer::visitIfStmt(const std::shared_ptr<If>& stmt) {
    evaluate(stmt->condition);
    check(stmt->thenBranch);
    if (stmt->elseBranch != nullptr) {
        check(stmt->elseBranch);
    }
}

void SemanticAnalyzer::visitWhileStmt(const std::shared_ptr<While>& stmt) {
    evaluate(stmt->condition);
    check(stmt->body);
}

void SemanticAnalyzer::visitFunctionStmt(const std::shared_ptr<Function>& stmt) {
    // In a real compiler, we'd handle function declarations here
}

void SemanticAnalyzer::visitReturnStmt(const std::shared_ptr<Return>& stmt) {
    if (stmt->value != nullptr) {
        evaluate(stmt->value);
    }
}

void SemanticAnalyzer::visitStructStmt(const std::shared_ptr<Struct>& stmt) {
    // In a real compiler, we'd handle struct declarations here
}


void SemanticAnalyzer::executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> blockEnvironment) {
    std::shared_ptr<Environment> previous = this->environment;
    try {
        this->environment = blockEnvironment;
        for (const auto& statement : statements) {
            check(statement);
        }
    } catch (...) {
        this->environment = previous;
        throw;
    }
    this->environment = previous;
}

void SemanticAnalyzer::check(const std::shared_ptr<Stmt>& stmt) {
    if (stmt) {
        stmt->accept(*this);
    }
}

std::any SemanticAnalyzer::evaluate(const std::shared_ptr<Expr>& expr) {
    if (expr) {
        return expr->accept(*this);
    }
    return nullptr;
}
