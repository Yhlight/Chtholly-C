#include "CodeGen.h"
#include <stdexcept>

std::string CodeGen::generate(const std::vector<std::shared_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        if(statement) {
            generate(statement);
        }
    }

    std::stringstream final_code;
    for (const auto& header : headers) {
        final_code << "#include " << header << "\n";
    }
    final_code << "\n";
    final_code << declarations.str() << "\n";
    final_code << "int main() {\n";
    final_code << main_body.str();
    final_code << "    return 0;\n";
    final_code << "}\n";

    return final_code.str();
}

void CodeGen::generate(const std::shared_ptr<Stmt>& stmt) {
    stmt->accept(*this);
}

std::string CodeGen::generate(const std::shared_ptr<Expr>& expr) {
    return std::any_cast<std::string>(expr->accept(*this));
}

// Statement visitors
void CodeGen::visitExpressionStmt(const std::shared_ptr<ExpressionStmt>& stmt) {
    main_body << "    " << generate(stmt->expression) << ";\n";
}

void CodeGen::visitPrintStmt(const std::shared_ptr<PrintStmt>& stmt) {
    headers.insert("<iostream>");
    main_body << "    std::cout << " << generate(stmt->expression) << " << std::endl;\n";
}

void CodeGen::visitVarStmt(const std::shared_ptr<VarStmt>& stmt) {
    std::string type = (stmt->keyword.type == TokenType::LET) ? "const auto" : "auto";
    main_body << "    " << type << " " << stmt->name.lexeme;
    if (stmt->initializer) {
        main_body << " = " << generate(stmt->initializer);
    }
    main_body << ";\n";
}

// Expression visitors
std::any CodeGen::visitAssignExpr(const std::shared_ptr<Assign>& expr) {
    return expr->name.lexeme + " = " + generate(expr->value);
}

std::any CodeGen::visitBinaryExpr(const std::shared_ptr<Binary>& expr) {
    return "(" + generate(expr->left) + " " + expr->op.lexeme + " " + generate(expr->right) + ")";
}

std::any CodeGen::visitGroupingExpr(const std::shared_ptr<Grouping>& expr) {
    return "(" + generate(expr->expression) + ")";
}

std::any CodeGen::visitLiteralExpr(const std::shared_ptr<Literal>& expr) {
    if (expr->value.has_value()) {
        if (expr->value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(expr->value));
        }
        if (expr->value.type() == typeid(std::string)) {
            headers.insert("<string>");
            return "\"" + std::any_cast<std::string>(expr->value) + "\"";
        }
        if (expr->value.type() == typeid(bool)) {
            return std::any_cast<bool>(expr->value) ? "true" : "false";
        }
    }
    return "nullptr";
}

std::any CodeGen::visitUnaryExpr(const std::shared_ptr<Unary>& expr) {
    return "(" + expr->op.lexeme + generate(expr->right) + ")";
}

std::any CodeGen::visitVariableExpr(const std::shared_ptr<Variable>& expr) {
    return expr->name.lexeme;
}
