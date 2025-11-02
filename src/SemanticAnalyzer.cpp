#include "../include/SemanticAnalyzer.h"

void Scope::declare(const std::string& name, bool is_mutable) {
    var_states[name] = BorrowState{is_mutable};
}

bool Scope::is_declared(const std::string& name) {
    return var_states.count(name);
}

bool Scope::is_mutable(const std::string& name) {
    if (var_states.count(name)) {
        return var_states[name].is_mutable;
    }
    return false;
}

bool Scope::is_moved(const std::string& name) {
    if (var_states.count(name)) {
        return var_states[name].moved;
    }
    return false;
}

void Scope::move(const std::string& name) {
    if (var_states.count(name)) {
        var_states[name].moved = true;
    }
}

bool Scope::is_borrowed(const std::string& name) {
    if (var_states.count(name)) {
        return var_states[name].immutable_borrows > 0 || var_states[name].mutable_borrow;
    }
    return false;
}

void Scope::borrow(const std::string& name, bool is_mutable) {
    if (var_states.count(name)) {
        if (is_mutable) {
            var_states[name].mutable_borrow = true;
        } else {
            var_states[name].immutable_borrows++;
        }
    }
}

void Scope::unborrow(const std::string& name, bool is_mutable) {
    if (var_states.count(name)) {
        if (is_mutable) {
            var_states[name].mutable_borrow = false;
        } else {
            var_states[name].immutable_borrows--;
        }
    }
}

bool Scope::is_mutably_borrowed(const std::string& name) {
    if (var_states.count(name)) {
        return var_states[name].mutable_borrow;
    }
    return false;
}

std::vector<std::string> SemanticAnalyzer::analyze(const std::vector<std::shared_ptr<Stmt>>& statements) {
    errors.clear();
    scopes.emplace_back();
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    return errors;
}

void SemanticAnalyzer::begin_scope() {
    scopes.emplace_back();
}

void SemanticAnalyzer::end_scope() {
    scopes.pop_back();
}

void SemanticAnalyzer::visitExpressionStmt(std::shared_ptr<Expression> stmt) {
    stmt->expression->accept(*this);
}

void SemanticAnalyzer::visitVarStmt(std::shared_ptr<Var> stmt) {
    if (stmt->initializer) {
        stmt->initializer->accept(*this);

        std::shared_ptr<Variable> var_node = nullptr;
        bool is_borrow = false;

        if (auto unary = std::dynamic_pointer_cast<Unary>(stmt->initializer)) {
            if (unary->op.type == TokenType::AMPERSAND) {
                 var_node = std::dynamic_pointer_cast<Variable>(unary->right);
                 is_borrow = true;
            }
        } else if (auto var = std::dynamic_pointer_cast<Variable>(stmt->initializer)) {
            var_node = var;
        }

        if (var_node) {
            if (is_borrow) {
                if (stmt->type && stmt->type->is_ref) {
                    if (stmt->type->is_mut_ref) {
                        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
                            if (it->is_borrowed(var_node->name.lexeme)) {
                                errors.push_back("Error: cannot mutably borrow '" + var_node->name.lexeme + "' because it is already borrowed");
                                break;
                            }
                        }
                        scopes.back().borrow(var_node->name.lexeme, true);
                    } else { // immutable borrow
                        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
                            if (it->is_mutably_borrowed(var_node->name.lexeme)) {
                                errors.push_back("Error: cannot borrow '" + var_node->name.lexeme + "' as immutable because it is also borrowed as mutable");
                                break;
                            }
                        }
                        scopes.back().borrow(var_node->name.lexeme, false);
                    }
                }
            } else { // it's a move
                scopes.back().move(var_node->name.lexeme);
            }
        }
    }
    scopes.back().declare(stmt->name.lexeme, stmt->isMutable);
}

void SemanticAnalyzer::visitBlockStmt(std::shared_ptr<Block> stmt) {
    begin_scope();
    for (const auto& statement : stmt->statements) {
        statement->accept(*this);
    }
    end_scope();
}

void SemanticAnalyzer::visitFuncStmt(std::shared_ptr<Func> stmt) {
    begin_scope();
    for (const auto& param : stmt->params) {
        scopes.back().declare(param.name.lexeme, false); // function parameters are immutable by default
    }
    for (const auto& statement : stmt->body) {
        statement->accept(*this);
    }
    end_scope();
}

void SemanticAnalyzer::visitReturnStmt(std::shared_ptr<Return> stmt) {
    if (stmt->value) {
        stmt->value->accept(*this);
    }
}

void SemanticAnalyzer::visitStructStmt(std::shared_ptr<Struct> stmt) {
    // Methods are not yet implemented
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
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->is_moved(expr->name.lexeme)) {
            errors.push_back("Error: use of moved value '" + expr->name.lexeme + "'");
            return nullptr;
        }
    }
    return nullptr;
}

std::any SemanticAnalyzer::visitAssignExpr(std::shared_ptr<Assign> expr) {
    expr->value->accept(*this);

    Scope* var_scope = nullptr;
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->is_declared(expr->name.lexeme)) {
            var_scope = &(*it);
            break;
        }
    }

    if (!var_scope) {
        errors.push_back("Error: assignment to undeclared variable '" + expr->name.lexeme + "'");
        return nullptr;
    }

    if (!var_scope->is_mutable(expr->name.lexeme)) {
        errors.push_back("Error: cannot assign to immutable variable '" + expr->name.lexeme + "'");
        return nullptr;
    }

    if (var_scope->is_borrowed(expr->name.lexeme)) {
        errors.push_back("Error: cannot assign to '" + expr->name.lexeme + "' because it is borrowed");
        return nullptr;
    }

    return nullptr;
}

std::any SemanticAnalyzer::visitCallExpr(std::shared_ptr<Call> expr) {
    for (const auto& arg : expr->arguments) {
        arg->accept(*this);
        if (auto var = std::dynamic_pointer_cast<Variable>(arg)) {
            if (arg->type) {
                if (arg->type->is_mut_ref) {
                    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
                        if (it->is_borrowed(var->name.lexeme)) {
                            errors.push_back("Error: cannot mutably borrow '" + var->name.lexeme + "' because it is already borrowed");
                            break;
                        }
                    }
                    scopes.back().borrow(var->name.lexeme, true);
                } else if (arg->type->is_ref) {
                    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
                        if (it->is_mutably_borrowed(var->name.lexeme)) {
                            errors.push_back("Error: cannot borrow '" + var->name.lexeme + "' as immutable because it is also borrowed as mutable");
                            break;
                        }
                    }
                    scopes.back().borrow(var->name.lexeme, false);
                } else {
                    scopes.back().move(var->name.lexeme);
                }
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
