#include "CodeGen.h"

std::string CodeGen::generate(const std::vector<std::shared_ptr<Stmt>>& statements) {
    std::string code;
    for (const auto& stmt : statements) {
        if (stmt) {
            code += stmt->accept(*this);
        }
    }
    return code;
}

std::string CodeGen::visitFuncStmt(const std::shared_ptr<Func>& stmt) {
    std::string code = "auto " + stmt->name.lexeme + " = [](";
    for (size_t i = 0; i < stmt->params.size(); ++i) {
        if (stmt->params[i].type) {
            code += stmt->params[i].type->name.lexeme + " " + stmt->params[i].name.lexeme;
        } else {
            code += "auto " + stmt->params[i].name.lexeme;
        }
        if (i < stmt->params.size() - 1) {
            code += ", ";
        }
    }
    code += ")";
    if (stmt->returnType) {
        code += " -> " + stmt->returnType->name.lexeme;
    }
    code += " {\n";
    for (const auto& statement : stmt->body) {
        code += statement->accept(*this);
    }
    code += "};\n";
    return code;
}

std::string CodeGen::visitReturnStmt(const std::shared_ptr<Return>& stmt) {
    std::string code = "return";
    if (stmt->value) {
        code += " " + stmt->value->accept(*this);
    }
    code += ";\n";
    return code;
}

std::string CodeGen::visitCallExpr(const std::shared_ptr<Call>& expr) {
    std::string code = expr->callee->accept(*this) + "(";
    for (size_t i = 0; i < expr->arguments.size(); ++i) {
        code += expr->arguments[i]->accept(*this);
        if (i < expr->arguments.size() - 1) {
            code += ", ";
        }
    }
    code += ")";
    return code;
}

std::string CodeGen::visitWhileStmt(const std::shared_ptr<While>& stmt) {
    std::string code = "while (" + stmt->condition->accept(*this) + ") " + stmt->body->accept(*this);
    return code;
}

std::string CodeGen::visitBinaryExpr(const std::shared_ptr<Binary>& expr) {
    return expr->left->accept(*this) + " " + expr->op.lexeme + " " + expr->right->accept(*this);
}

std::string CodeGen::visitGroupingExpr(const std::shared_ptr<Grouping>& expr) {
    return "(" + expr->expression->accept(*this) + ")";
}

std::string CodeGen::visitLiteralExpr(const std::shared_ptr<Literal>& expr) {
    if (expr->type == TokenType::STRING) {
        return "\"" + expr->value + "\"";
    }
    return expr->value;
}

std::string CodeGen::visitUnaryExpr(const std::shared_ptr<Unary>& expr) {
    return expr->op.lexeme + expr->right->accept(*this);
}

std::string CodeGen::visitVariableExpr(const std::shared_ptr<Variable>& expr) {
    return expr->name.lexeme;
}

std::string CodeGen::visitAssignExpr(const std::shared_ptr<Assign>& expr) {
    return expr->name.lexeme + " = " + expr->value->accept(*this);
}

std::string CodeGen::visitExpressionStmt(const std::shared_ptr<Expression>& stmt) {
    return stmt->expression->accept(*this) + ";\n";
}

std::string CodeGen::visitPrintStmt(const std::shared_ptr<Print>& stmt) {
    return "std::cout << " + stmt->expression->accept(*this) + " << std::endl;\n";
}

std::string CodeGen::visitVarStmt(const std::shared_ptr<Var>& stmt) {
    std::string type_str = "auto";
    if (stmt->type) {
        type_str = stmt->type->name.lexeme;
    }
    std::string code = (stmt->is_mutable ? "" : "const ") + type_str + " " + stmt->name.lexeme;
    if (stmt->initializer) {
        code += " = " + stmt->initializer->accept(*this);
    }
    code += ";\n";
    return code;
}

std::string CodeGen::visitBlockStmt(const std::shared_ptr<Block>& stmt) {
    std::string code = "{\n";
    for (const auto& statement : stmt->statements) {
        code += statement->accept(*this);
    }
    code += "}\n";
    return code;
}

std::string CodeGen::visitIfStmt(const std::shared_ptr<If>& stmt) {
    std::string code = "if (" + stmt->condition->accept(*this) + ") " + stmt->thenBranch->accept(*this);
    if (stmt->elseBranch) {
        code += " else " + stmt->elseBranch->accept(*this);
    }
    return code;
}
