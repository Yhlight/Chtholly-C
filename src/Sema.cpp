#include "Sema.h"
#include "SymbolTable.h"
#include <stdexcept>

Sema::Sema() {
    symbol_table_.enter_scope();
}

bool Sema::is_copy_type(const std::shared_ptr<Type>& type) {
    if (treat_int_as_move && dynamic_cast<IntType*>(type.get())) {
        return false;
    }
    return dynamic_cast<IntType*>(type.get()) || dynamic_cast<DoubleType*>(type.get());
}

std::shared_ptr<Type> Sema::visit(ExprAST& ast) {
    if (auto* var_decl = dynamic_cast<VarDeclAST*>(&ast)) {
        return visit(*var_decl);
    } else if (auto* bin_expr = dynamic_cast<BinaryExprAST*>(&ast)) {
        return visit(*bin_expr);
    } else if (auto* var_expr = dynamic_cast<VariableExprAST*>(&ast)) {
        return visit(*var_expr);
    } else if (auto* block_expr = dynamic_cast<BlockExprAST*>(&ast)) {
        return visit(*block_expr);
    } else if (auto* num_expr = dynamic_cast<NumberExprAST*>(&ast)) {
        return visit(*num_expr);
    } else if (auto* call_expr = dynamic_cast<CallExprAST*>(&ast)) {
        return visit(*call_expr);
    }
    return nullptr;
}

std::shared_ptr<Type> Sema::visit(VarDeclAST& ast) {
    auto init_type = visit(*ast.get_init());

    if (ast.type && init_type->to_string() != ast.type->to_string()) {
        throw std::runtime_error("Initializer type does not match variable type");
    }

    if (!ast.type) {
        ast.type = init_type;
    }

    if (auto* rhs = dynamic_cast<VariableExprAST*>(ast.get_init())) {
        auto* rhs_symbol = symbol_table_.lookup(rhs->get_name());
        if (rhs_symbol && !is_copy_type(rhs_symbol->type)) {
            rhs_symbol->state = OwnershipState::Moved;
        }
    }

    std::string name = ast.get_name();
    if (symbol_table_.lookup(name)) {
        throw std::runtime_error("Variable already declared: " + name);
    }
    symbol_table_.insert(name, {name, OwnershipState::Owned, ast.type});
    return ast.type;
}

std::shared_ptr<Type> Sema::visit(BinaryExprAST& ast) {
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

        auto rhs_type = visit(*ast.get_rhs());

        if (symbol->type && symbol->type->to_string() != rhs_type->to_string()) {
            throw std::runtime_error("Type mismatch in assignment");
        }

        if (auto* rhs_var = dynamic_cast<VariableExprAST*>(ast.get_rhs())) {
            auto* rhs_symbol = symbol_table_.lookup(rhs_var->get_name());
            if (rhs_symbol && !is_copy_type(rhs_symbol->type)) {
                rhs_symbol->state = OwnershipState::Moved;
            }
        }

        ast.type = symbol->type;
        return ast.type;
    }

    auto lhs_type = visit(*ast.get_lhs());
    auto rhs_type = visit(*ast.get_rhs());

    if (lhs_type->to_string() != rhs_type->to_string()) {
        throw std::runtime_error("Type mismatch in binary expression");
    }

    ast.type = lhs_type;
    return ast.type;
}

std::shared_ptr<Type> Sema::visit(VariableExprAST& ast) {
    auto* symbol = symbol_table_.lookup(ast.get_name());
    if (!symbol) {
        throw std::runtime_error("Undeclared variable: " + ast.get_name());
    }
    if (symbol->state == OwnershipState::Moved) {
        throw std::runtime_error("Use of moved variable: " + ast.get_name());
    }
    ast.type = symbol->type;
    return ast.type;
}

std::shared_ptr<Type> Sema::visit(BlockExprAST& ast) {
    std::shared_ptr<Type> last_type;
    for (const auto& expr : ast.get_expressions()) {
        last_type = visit(*expr);
    }
    ast.type = last_type;
    return ast.type;
}

std::shared_ptr<Type> Sema::visit(NumberExprAST& ast) {
    ast.type = std::make_shared<IntType>();
    return ast.type;
}

std::shared_ptr<Type> Sema::visit(CallExprAST& ast) {
    // We don't have function definitions yet, so we can't do much here.
    // We'll just assume the function returns an int for now.
    ast.type = std::make_shared<IntType>();
    return ast.type;
}
