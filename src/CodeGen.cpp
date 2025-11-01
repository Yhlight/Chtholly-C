#include "../include/CodeGen.h"

std::string CodeGen::generate(const std::vector<std::shared_ptr<Stmt>>& statements) {
    source.clear();
    for (const auto& stmt : statements) {
        execute(stmt);
    }
    return source;
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
    source += stmt->isMutable ? "auto " : "const auto ";
    source += stmt->name.lexeme;
    if (stmt->initializer) {
        source += " = " + evaluate(stmt->initializer);
    }
    source += ";\n";
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
    source += "auto " + stmt->name.lexeme + " = [](";
    for (const auto& param : stmt->params) {
        source += "auto " + param.lexeme + ", ";
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
