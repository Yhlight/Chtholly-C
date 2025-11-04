#include <SemanticAnalyzer.h>
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer() {
    environment = std::make_shared<Environment>();
}

void SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
}

void SemanticAnalyzer::visit(const BinaryExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void SemanticAnalyzer::visit(const GroupingExpr& expr) {
    expr.expression->accept(*this);
}

void SemanticAnalyzer::visit(const LiteralExpr& expr) {
    // Literals are always valid.
}

void SemanticAnalyzer::visit(const UnaryExpr& expr) {
    expr.right->accept(*this);
}

void SemanticAnalyzer::visit(const VariableExpr& expr) {
    if (!environment->isDefined(expr.name.lexeme)) {
        error(expr.name, "Undefined variable '" + expr.name.lexeme + "'.");
    }
}

void SemanticAnalyzer::visit(const AssignmentExpr& expr) {
    if (!environment->isDefined(expr.name.lexeme)) {
        error(expr.name, "Undefined variable '" + expr.name.lexeme + "'.");
    }
    if (!environment->isMutable(expr.name.lexeme)) {
        error(expr.name, "Cannot assign to immutable variable '" + expr.name.lexeme + "'.");
    }
    expr.value->accept(*this);
}

void SemanticAnalyzer::visit(const CallExpr& expr) {
    expr.callee->accept(*this);
    for (const auto& arg : expr.arguments) {
        arg->accept(*this);
    }
}

void SemanticAnalyzer::visit(const BlockStmt& stmt) {
    auto previous = environment;
    environment = std::make_shared<Environment>(previous);
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    environment = previous;
}

void SemanticAnalyzer::visit(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
}

void SemanticAnalyzer::visit(const FunctionStmt& stmt) {
    if (environment->isDefined(stmt.name.lexeme)) {
        error(stmt.name, "Function '" + stmt.name.lexeme + "' already defined.");
    }
    environment->define(stmt.name.lexeme, false);

    auto previous = environment;
    environment = std::make_shared<Environment>(previous);
    for (const auto& param : stmt.params) {
        if (environment->isDefined(param.lexeme)) {
            error(param, "Parameter '" + param.lexeme + "' already defined.");
        }
        environment->define(param.lexeme, false);
    }
    stmt.body->accept(*this);
    environment = previous;
}

void SemanticAnalyzer::visit(const IfStmt& stmt) {
    stmt.condition->accept(*this);
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }
}

void SemanticAnalyzer::visit(const ReturnStmt& stmt) {
    if (stmt.value) {
        stmt.value->accept(*this);
    }
}

void SemanticAnalyzer::visit(const VarDeclStmt& stmt) {
    if (environment->isDefined(stmt.name.lexeme)) {
        error(stmt.name, "Variable '" + stmt.name.lexeme + "' already defined in this scope.");
    }
    environment->define(stmt.name.lexeme, stmt.isMutable);
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }
}

void SemanticAnalyzer::visit(const WhileStmt& stmt) {
    stmt.condition->accept(*this);
    stmt.body->accept(*this);
}

void SemanticAnalyzer::error(const Token& token, const std::string& message) {
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << "Error at end: " << message << std::endl;
    } else {
        std::cerr << "Error at '" << token.lexeme << "' (line " << token.line << "): " << message << std::endl;
    }
}
