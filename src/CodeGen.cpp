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

std::string CodeGen::visitInstantiationExpr(const std::shared_ptr<Instantiation>& expr) {
    std::string code = expr->name.lexeme + "{";
    for (size_t i = 0; i < expr->fields.size(); ++i) {
        code += "." + expr->fields[i].first.lexeme + " = " + expr->fields[i].second->accept(*this);
        if (i < expr->fields.size() - 1) {
            code += ", ";
        }
    }
    code += "}";
    return code;
}

std::string CodeGen::visitReferenceExpr(const std::shared_ptr<Reference>& expr) {
    return "&" + expr->right->accept(*this);
}

std::string CodeGen::visitDereferenceExpr(const std::shared_ptr<Dereference>& expr) {
    return "*" + expr->right->accept(*this);
}

std::string CodeGen::visitGetExpr(const std::shared_ptr<Get>& expr) {
    if (auto self = std::dynamic_pointer_cast<Self>(expr->object)) {
        return self->accept(*this) + expr->name.lexeme;
    }
    return expr->object->accept(*this) + "." + expr->name.lexeme;
}

std::string CodeGen::visitSetExpr(const std::shared_ptr<Set>& expr) {
    return expr->object->accept(*this) + "." + expr->name.lexeme + " = " + expr->value->accept(*this);
}

std::string CodeGen::visitSelfExpr(const std::shared_ptr<Self>& expr) {
    return "this->";
}

std::string CodeGen::visitStructStmt(const std::shared_ptr<Struct>& stmt) {
    std::string code = "struct " + stmt->name.lexeme + " {\n";

    for (const auto& field : stmt->fields) {
        code += field->accept(*this);
    }

    for (const auto& method : stmt->methods) {
        std::string returnType = "void";
        if (method->returnType) {
            returnType = method->returnType->name.lexeme;
        }
        code += "    " + returnType + " " + method->name.lexeme + "(";
        for (size_t i = 0; i < method->params.size(); ++i) {
            if (method->params[i].type) {
                code += method->params[i].type->name.lexeme + " " + method->params[i].name.lexeme;
            } else {
                code += "auto " + method->params[i].name.lexeme;
            }
            if (i < method->params.size() - 1) {
                code += ", ";
            }
        }
        code += ") {\n";
        for (const auto& statement : method->body) {
            code += "        " + statement->accept(*this);
        }
        code += "    }\n";
    }

    code += "};\n";
    return code;
}

std::string CodeGen::visitSwitchStmt(const std::shared_ptr<Switch>& stmt) {
    std::string code = "switch (" + stmt->condition->accept(*this) + ") {\n";
    for (const auto& case_stmt : stmt->cases) {
        code += "case " + case_stmt->condition->accept(*this) + ":\n";
        code += case_stmt->body->accept(*this);
    }
    code += "}\n";
    return code;
}

std::string CodeGen::visitBreakStmt(const std::shared_ptr<Break>& stmt) {
    return "break;\n";
}

std::string CodeGen::visitFallthroughStmt(const std::shared_ptr<Fallthrough>& stmt) {
    return "[[fallthrough]];\n";
}

std::string CodeGen::visitForStmt(const std::shared_ptr<For>& stmt) {
    std::string code = "for (";
    if (stmt->initializer) {
        if (auto varStmt = std::dynamic_pointer_cast<Var>(stmt->initializer)) {
            code += visitVarStmt(varStmt, false);
        } else if (auto exprStmt = std::dynamic_pointer_cast<Expression>(stmt->initializer)) {
            code += visitExpressionStmt(exprStmt, false);
        }
    }
    code += "; ";
    if (stmt->condition) {
        code += stmt->condition->accept(*this);
    }
    code += "; ";
    if (stmt->increment) {
        code += stmt->increment->accept(*this);
    }
    code += ") " + stmt->body->accept(*this);
    return code;
}

std::string CodeGen::visitFuncStmt(const std::shared_ptr<Func>& stmt) {
    std::string code = "auto " + stmt->name.lexeme + " = [](";
    for (size_t i = 0; i < stmt->params.size(); ++i) {
        if (stmt->params[i].type) {
            std::string type_str;
            if (stmt->params[i].type->name.lexeme == "string") {
                type_str = "std::string";
            } else {
                type_str = stmt->params[i].type->name.lexeme;
            }
            if (stmt->params[i].type->is_ref) {
                if (stmt->params[i].type->is_mut_ref) {
                    type_str += "&";
                } else {
                    type_str = "const " + type_str + "&";
                }
            }
            code += type_str + " " + stmt->params[i].name.lexeme;
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
        if (auto ref = std::dynamic_pointer_cast<Reference>(expr->arguments[i])) {
            code += ref->right->accept(*this);
        } else {
            code += expr->arguments[i]->accept(*this);
        }
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
    return expr->target->accept(*this) + " = " + expr->value->accept(*this);
}

std::string CodeGen::visitExpressionStmt(const std::shared_ptr<Expression>& stmt) {
    return visitExpressionStmt(stmt, true);
}

std::string CodeGen::visitVarStmt(const std::shared_ptr<Var>& stmt) {
    return visitVarStmt(stmt, true);
}

std::string CodeGen::visitExpressionStmt(const std::shared_ptr<Expression>& stmt, bool semicolon) {
    std::string code = stmt->expression->accept(*this);
    if (semicolon) {
        code += ";\n";
    }
    return code;
}

std::string CodeGen::visitVarStmt(const std::shared_ptr<Var>& stmt, bool semicolon) {
    std::string type_str = "auto";
    if (stmt->type) {
        if (stmt->type->name.lexeme == "string") {
            type_str = "std::string";
        } else {
            type_str = stmt->type->name.lexeme;
        }
        if (stmt->type->is_ref) {
            type_str += "&";
        }
    }
    std::string code = (stmt->is_mutable ? "" : "const ") + type_str + " " + stmt->name.lexeme;
    if (stmt->initializer) {
        code += " = " + stmt->initializer->accept(*this);
    }
    if (semicolon) {
        code += ";\n";
    }
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
