#include "Sema.h"
#include "SymbolTable.h"
#include <stdexcept>

Sema::Sema() {
    symbol_table_.enter_scope();
}

void Sema::visit(ExprAST& ast) {
    if (auto* var_decl = dynamic_cast<VarDeclAST*>(&ast)) {
        visit(*var_decl);
    } else if (auto* bin_expr = dynamic_cast<BinaryExprAST*>(&ast)) {
        visit(*bin_expr);
    } else if (auto* var_expr = dynamic_cast<VariableExprAST*>(&ast)) {
        visit(*var_expr);
    } else if (auto* block_expr = dynamic_cast<BlockExprAST*>(&ast)) {
        visit(*block_expr);
    } else if (auto* num_expr = dynamic_cast<NumberExprAST*>(&ast)) {
        visit(*num_expr);
    } else if (auto* call_expr = dynamic_cast<CallExprAST*>(&ast)) {
        visit(*call_expr);
    }
}

void Sema::visit(VarDeclAST& ast) {
    visit(*ast.get_init());

    if (auto* rhs = dynamic_cast<VariableExprAST*>(ast.get_init())) {
        auto* rhs_symbol = symbol_table_.lookup(rhs->get_name());
        if (rhs_symbol) {
            rhs_symbol->state = OwnershipState::Moved;
        }
    }

    std::string name = ast.get_name();
    if (symbol_table_.lookup(name)) {
        throw std::runtime_error("Variable already declared: " + name);
    }
    symbol_table_.insert(name, {name, OwnershipState::Owned});
}

void Sema::visit(BinaryExprAST& ast) {
    if (ast.is_assignment()) {
        auto* lhs = dynamic_cast<VariableExprAST*>(ast.get_lhs());
        if (!lhs) {
            throw std::runtime_error("Assignment to non-variable");
        }
        auto* symbol = symbol_table_.lookup(lhs->get_name());
        if (!symbol) {
            throw std::runtime_error("Undeclared variable: " + lhs->get_name());
        }
        if (symbol->state == OwnershipState::Moved) {
            throw std::runtime_error("Assignment to moved variable: " + lhs->get_name());
        }

        visit(*ast.get_rhs());

        if (auto* rhs = dynamic_cast<VariableExprAST*>(ast.get_rhs())) {
            auto* rhs_symbol = symbol_table_.lookup(rhs->get_name());
            if (rhs_symbol) {
                rhs_symbol->state = OwnershipState::Moved;
            }
        }
    } else {
        visit(*ast.get_lhs());
        visit(*ast.get_rhs());
    }
}

void Sema::visit(VariableExprAST& ast) {
    auto* symbol = symbol_table_.lookup(ast.get_name());
    if (!symbol) {
        throw std::runtime_error("Undeclared variable: " + ast.get_name());
    }
    if (symbol->state == OwnershipState::Moved) {
        throw std::runtime_error("Use of moved variable: " + ast.get_name());
    }
}

void Sema::visit(BlockExprAST& ast) {
    for (const auto& expr : ast.get_expressions()) {
        visit(*expr);
    }
}

void Sema::visit(NumberExprAST& ast) {
    // Nothing to do
}

void Sema::visit(CallExprAST& ast) {
    for (const auto& arg : ast.get_args()) {
        visit(*arg);
    }
}
