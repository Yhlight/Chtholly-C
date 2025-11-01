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
    out << "}";
}

void CodeGen::visit(const IfStmt& stmt) {
    out << "if (" << stmt.condition->accept(*this) << ") ";
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        out << " else ";
        stmt.elseBranch->accept(*this);
    }
    out << "\n";
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
    out << "\n";
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

std::string CodeGen::visit(const LambdaExpr& expr) {
    std::stringstream lambda_out;
    lambda_out << "[](";
    for (size_t i = 0; i < expr.params.size(); ++i) {
        lambda_out << expr.params[i].type.lexeme << " " << expr.params[i].name.lexeme;
        if (i < expr.params.size() - 1) {
            lambda_out << ", ";
        }
    }
    lambda_out << ")";
    if (expr.returnType) {
        lambda_out << " -> " << expr.returnType->lexeme;
    }
    lambda_out << " ";

    // Temporarily redirect the main output stream to capture the body
    std::stringstream body_out;
    auto* old_out = &out;
    out.swap(body_out);

    expr.body->accept(*this);

    // Restore the main output stream
    out.swap(body_out);

    // Get the body content and append it to the lambda expression
    std::string body_str = body_out.str();
    lambda_out << body_str;

    return lambda_out.str();
}

void CodeGen::visit(const SwitchStmt& stmt) {
    out << "switch (" << stmt.condition->accept(*this) << ") {\n";
    for (const auto& caseStmt : stmt.cases) {
        caseStmt->accept(*this);
    }
    out << "}\n";
}

void CodeGen::visit(const CaseStmt& stmt) {
    if (stmt.condition) {
        out << "case " << stmt.condition->accept(*this) << ":\n";
    } else {
        out << "default:\n";
    }
    stmt.body->accept(*this);
    out << "\n";
}

void CodeGen::visit(const BreakStmt& stmt) {
    out << "break;\n";
}

void CodeGen::visit(const FallthroughStmt& stmt) {
    out << "[[fallthrough]];\n";
}

void CodeGen::visit(const StructDeclStmt& stmt) {
    out << "struct " << stmt.name.lexeme << " {\n";
    for (const auto& member : stmt.members) {
        if (member.visibility == StructDeclStmt::Visibility::PRIVATE) {
            out << "private:\n";
        }
        out << member.type.lexeme << " " << member.name.lexeme << ";\n";
    }
    for (const auto& method : stmt.methods) {
        method->accept(*this);
    }
    out << "};\n";
}

std::string CodeGen::visit(const StructInitExpr& expr) {
    std::stringstream ss;
    ss << expr.name.lexeme << "{ ";
    for (size_t i = 0; i < expr.members.size(); ++i) {
        ss << "." << expr.members[i].name.lexeme << " = " << expr.members[i].initializer->accept(*this);
        if (i < expr.members.size() - 1) {
            ss << ", ";
        }
    }
    ss << " }";
    return ss.str();
}

std::string CodeGen::visit(const MemberAccessExpr& expr) {
    return expr.object->accept(*this) + "." + expr.name.lexeme;
}

} // namespace chtholly
