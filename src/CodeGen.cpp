#include "CodeGen.h"
#include "AST.h"

namespace chtholly {

std::string CodeGen::generate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    return out.str();
}

std::string CodeGen::generateVarDecl(const VarDeclStmt& stmt) {
    std::stringstream temp_out;
    if (stmt.keyword.type == TokenType::LET) {
        temp_out << "const ";
    }
    temp_out << "auto " << stmt.name.lexeme;
    if (stmt.initializer) {
        temp_out << " = " << stmt.initializer->accept(*this);
    }
    return temp_out.str();
}

void CodeGen::visit(const VarDeclStmt& stmt) {
    out << generateVarDecl(stmt) << ";\n";
}

void CodeGen::visit(const ExprStmt& stmt) {
    out << stmt.expression->accept(*this) << ";\n";
}

void CodeGen::visit(const BlockStmt& stmt) {
    out << "{\n";
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    out << "}\n";
}

void CodeGen::visit(const IfStmt& stmt) {
    out << "if (" << stmt.condition->accept(*this) << ") ";
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        out << "else ";
        stmt.elseBranch->accept(*this);
    }
}

void CodeGen::visit(const ForStmt& stmt) {
    out << "for (";
    if (stmt.initializer) {
        if (auto* varDecl = dynamic_cast<const VarDeclStmt*>(stmt.initializer.get())) {
            out << generateVarDecl(*varDecl);
        } else if (auto* exprStmt = dynamic_cast<const ExprStmt*>(stmt.initializer.get())) {
            out << exprStmt->expression->accept(*this);
        }
    }
    out << "; ";
    if (stmt.condition) {
        out << stmt.condition->accept(*this);
    }
    out << "; ";
    if (stmt.increment) {
        out << stmt.increment->accept(*this);
    }
    out << ") ";
    stmt.body->accept(*this);
}

void CodeGen::visit(const FuncDeclStmt& stmt) {
    out << "auto " << stmt.name.lexeme << "(";
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        out << stmt.params[i].type.lexeme << " " << stmt.params[i].name.lexeme;
        if (i < stmt.params.size() - 1) {
            out << ", ";
        }
    }
    out << ")";
    if (stmt.returnType) {
        out << " -> " << stmt.returnType->lexeme;
    }
    out << " ";
    stmt.body->accept(*this);
}

void CodeGen::visit(const ReturnStmt& stmt) {
    out << "return";
    if (stmt.value) {
        out << " " << stmt.value->accept(*this);
    }
    out << ";\n";
}

std::string CodeGen::visit(const LiteralExpr& expr) {
    return expr.token.lexeme;
}

std::string CodeGen::visit(const UnaryExpr& expr) {
    return "(" + expr.op.lexeme + expr.right->accept(*this) + ")";
}

std::string CodeGen::visit(const BinaryExpr& expr) {
    return "(" + expr.left->accept(*this) + " " + expr.op.lexeme + " " + expr.right->accept(*this) + ")";
}

std::string CodeGen::visit(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::string CodeGen::visit(const GroupingExpr& expr) {
    return expr.expression->accept(*this);
}

std::string CodeGen::visit(const CallExpr& expr) {
    std::string result = expr.callee->accept(*this) + "(";
    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        result += expr.arguments[i]->accept(*this);
        if (i < expr.arguments.size() - 1) {
            result += ", ";
        }
    }
    result += ")";
    return result;
}

} // namespace chtholly
