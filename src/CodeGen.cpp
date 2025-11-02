#include "../include/CodeGen.h"

CodeGen::CodeGen() {
    builtins["print"] = [this](const std::vector<std::shared_ptr<Expr>>& args) {
        std::string result = "std::cout << ";
        for (const auto& arg : args) {
            result += evaluate(arg) + " << ";
        }
        result.pop_back();
        result.pop_back();
        result.pop_back();
        result.pop_back();
        result += " << std::endl";
        return result;
    };
}

std::string CodeGen::typeToString(const std::shared_ptr<Type>& type) {
    if (!type) {
        return "auto";
    }

    std::string type_str;
    if (type->kind == TypeKind::PRIMITIVE) {
        type_str = std::dynamic_pointer_cast<PrimitiveType>(type)->name;
    } else if (type->kind == TypeKind::ARRAY) {
        type_str = "std::vector<" + typeToString(std::dynamic_pointer_cast<ArrayType>(type)->element_type) + ">";
    } else {
        type_str = "auto";
    }

    if (type_str == "std::string") {
        headers.insert("<string>");
    }

    if (type->is_mut_ref) {
        type_str += "&";
    } else if (type->is_ref) {
        type_str += "&";
    }

    return type_str;
}

std::string CodeGen::generate(const std::vector<std::shared_ptr<Stmt>>& statements) {
    source.clear();
    headers.clear();
    headers.insert("<iostream>");

    for (const auto& stmt : statements) {
        execute(stmt);
    }

    std::string final_source;
    for (const auto& header : headers) {
        final_source += "#include " + header + "\n";
    }
    final_source += "\n";
    final_source += "int main() {\n";
    final_source += source;
    final_source += "return 0;\n";
    final_source += "}\n";

    return final_source;
}

std::string CodeGen::evaluate(const std::shared_ptr<Expr>& expr) {
    return std::any_cast<std::string>(expr->accept(*this));
}

void CodeGen::execute(const std::shared_ptr<Stmt>& stmt) {
    stmt->accept(*this);
}

// Expression visitors
std::any CodeGen::visitBinaryExpr(std::shared_ptr<Binary> expr) {
    return evaluate(expr->left) + " " + expr->op.lexeme + " " + evaluate(expr->right);
}

std::any CodeGen::visitGroupingExpr(std::shared_ptr<Grouping> expr) {
    return "(" + evaluate(expr->expression) + ")";
}

std::any CodeGen::visitLiteralExpr(std::shared_ptr<Literal> expr) {
    if (expr->value.type() == typeid(std::string)) {
        return "\"" + std::any_cast<std::string>(expr->value) + "\"";
    }
    if (expr->value.type() == typeid(bool)) {
        return std::any_cast<bool>(expr->value) ? "true" : "false";
    }
    if (expr->value.type() == typeid(nullptr_t)) {
        return "nullptr";
    }
    if (expr->value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(expr->value));
    }
    if (expr->value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(expr->value));
    }
    return "/* unhandled literal */";
}

std::any CodeGen::visitUnaryExpr(std::shared_ptr<Unary> expr) {
    if (expr->op.type == TokenType::AMPERSAND) {
        return evaluate(expr->right);
    }
    return expr->op.lexeme + evaluate(expr->right);
}

std::any CodeGen::visitVariableExpr(std::shared_ptr<Variable> expr) {
    return expr->name.lexeme;
}

std::any CodeGen::visitAssignExpr(std::shared_ptr<Assign> expr) {
    return expr->name.lexeme + " = " + evaluate(expr->value);
}

std::any CodeGen::visitCallExpr(std::shared_ptr<Call> expr) {
    std::string callee = evaluate(expr->callee);

    if (builtins.count(callee)) {
        return builtins[callee](expr->arguments);
    }

    std::string args;
    for (const auto& arg : expr->arguments) {
        args += evaluate(arg) + ", ";
    }
    if (!args.empty()) {
        args.pop_back();
        args.pop_back();
    }
    return callee + "(" + args + ")";
}

std::any CodeGen::visitGetExpr(std::shared_ptr<Get> expr) {
    return evaluate(expr->object) + "." + expr->name.lexeme;
}

std::any CodeGen::visitInstantiationExpr(std::shared_ptr<Instantiation> expr) {
    std::string fields;
    for (size_t i = 0; i < expr->fields.size(); ++i) {
        fields += "." + expr->fields[i].lexeme + " = " + evaluate(expr->values[i]) + ", ";
    }
    if (!fields.empty()) {
        fields.pop_back();
        fields.pop_back();
    }
    return expr->name.lexeme + "{" + fields + "}";
}


// Statement visitors
void CodeGen::visitExpressionStmt(std::shared_ptr<Expression> stmt) {
    source += evaluate(stmt->expression) + ";\n";
}

void CodeGen::visitVarStmt(std::shared_ptr<Var> stmt) {
    std::string type_str = typeToString(stmt->type);
    std::string var_decl;

    if (!stmt->isMutable) {
        var_decl += "const ";
    }
    var_decl += type_str + " " + stmt->name.lexeme;

    if (stmt->initializer) {
        var_decl += " = " + evaluate(stmt->initializer);
    }

    source += var_decl + ";\n";
}

void CodeGen::visitStructStmt(std::shared_ptr<Struct> stmt) {
    source += "struct " + stmt->name.lexeme + " {\n";
    for (const auto& field : stmt->fields) {
        execute(field);
    }
    source += "};\n";
}

void CodeGen::visitBlockStmt(std::shared_ptr<Block> stmt) {
    source += "{\n";
    for (const auto& statement : stmt->statements) {
        execute(statement);
    }
    source += "}\n";
}

void CodeGen::visitFuncStmt(std::shared_ptr<Func> stmt) {
    source += "auto " + stmt->name.lexeme + " = [&](";
    for (const auto& param : stmt->params) {
        source += typeToString(param.type) + " " + param.name.lexeme + ", ";
    }
    if (!stmt->params.empty()) {
        source.pop_back();
        source.pop_back();
    }
    source += ") {\n";
    for (const auto& statement : stmt->body) {
        execute(statement);
    }
    source += "};\n";
}

void CodeGen::visitReturnStmt(std::shared_ptr<Return> stmt) {
    source += "return";
    if (stmt->value) {
        source += " " + evaluate(stmt->value);
    }
    source += ";\n";
}
