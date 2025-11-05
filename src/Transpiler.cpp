#include "Transpiler.h"
#include <utility>
#include <stdexcept>

namespace chtholly {

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
    return out.str();
}

std::any Transpiler::visitBinaryExpr(const BinaryExpr& expr) {
    return std::any_cast<std::string>(expr.left->accept(*this)) + " " + expr.op.lexeme + " " + std::any_cast<std::string>(expr.right->accept(*this));
}

std::any Transpiler::visitUnaryExpr(const UnaryExpr& expr) {
    return expr.op.lexeme + std::any_cast<std::string>(expr.right->accept(*this));
}

std::any Transpiler::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::nullptr_t>(expr.value)) {
        return std::string("nullptr");
    }
    if (std::holds_alternative<std::string>(expr.value)) {
        return "\"" + std::get<std::string>(expr.value) + "\"";
    }
    if (std::holds_alternative<double>(expr.value)) {
        return std::to_string(std::get<double>(expr.value));
    }
    if (std::holds_alternative<long long>(expr.value)) {
        return std::to_string(std::get<long long>(expr.value));
    }
    if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    }
    if (std::holds_alternative<char>(expr.value)) {
        return "'" + std::string(1, std::get<char>(expr.value)) + "'";
    }
    return std::string("unknown literal");
}

std::any Transpiler::visitGroupingExpr(const GroupingExpr& expr) {
    return "(" + std::any_cast<std::string>(expr.expression->accept(*this)) + ")";
}

std::any Transpiler::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any Transpiler::visitAssignExpr(const AssignExpr& expr) {
    return expr.name.lexeme + " = " + std::any_cast<std::string>(expr.value->accept(*this));
}

// Placeholder for now
std::any Transpiler::visitCallExpr(const CallExpr& expr) {
    std::stringstream out;
    out << std::any_cast<std::string>(expr.callee->accept(*this)) << "(";
    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        out << std::any_cast<std::string>(expr.arguments[i]->accept(*this));
        if (i < expr.arguments.size() - 1) {
            out << ", ";
        }
    }
    out << ")";
    return out.str();
}
std::any Transpiler::visitLambdaExpr(const LambdaExpr& expr) { return nullptr; }
std::any Transpiler::visitGetExpr(const GetExpr& expr) { return nullptr; }
std::any Transpiler::visitSetExpr(const SetExpr& expr) { return nullptr; }
std::any Transpiler::visitSelfExpr(const SelfExpr& expr) { return nullptr; }
std::any Transpiler::visitBorrowExpr(const BorrowExpr& expr) { return nullptr; }
std::any Transpiler::visitDerefExpr(const DerefExpr& expr) { return nullptr; }

std::any Transpiler::visitBlockStmt(const BlockStmt& stmt) {
    out << "{\n";
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    out << "}\n";
    return nullptr;
}

std::any Transpiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    out << std::any_cast<std::string>(stmt.expression->accept(*this)) << ";\n";
    return nullptr;
}

std::any Transpiler::visitVarStmt(const VarStmt& stmt) {
    out << (stmt.isMutable ? "auto " : "const auto ") << stmt.name.lexeme;
    if (stmt.initializer) {
        out << " = " << std::any_cast<std::string>(stmt.initializer->accept(*this));
    }
    out << ";\n";
    return nullptr;
}

// Placeholder for now
std::any Transpiler::visitIfStmt(const IfStmt& stmt) {
    out << "if (" << std::any_cast<std::string>(stmt.condition->accept(*this)) << ") ";
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        out << "else ";
        stmt.elseBranch->accept(*this);
    }
    return nullptr;
}
std::any Transpiler::visitWhileStmt(const WhileStmt& stmt) {
    out << "while (" << std::any_cast<std::string>(stmt.condition->accept(*this)) << ") ";
    stmt.body->accept(*this);
    return nullptr;
}
std::any Transpiler::visitReturnStmt(const ReturnStmt& stmt) {
    out << "return";
    if (stmt.value) {
        out << " " << std::any_cast<std::string>(stmt.value->accept(*this));
    }
    out << ";\n";
    return nullptr;
}
std::any Transpiler::visitImportStmt(const ImportStmt& stmt) { return nullptr; }
std::any Transpiler::visitSwitchStmt(const SwitchStmt& stmt) {
    out << "{\n";
    out << "auto&& __switch_val = " << std::any_cast<std::string>(stmt.expression->accept(*this)) << ";\n";
    bool first_case = true;
    const CaseStmt* default_case = nullptr;

    for (size_t i = 0; i < stmt.cases.size(); ) {
        if (!stmt.cases[i]->value) {
            default_case = stmt.cases[i].get();
            i++;
            continue;
        }

        std::vector<const CaseStmt*> current_cases;
        current_cases.push_back(stmt.cases[i].get());

        bool has_fallthrough = false;
        if (const auto* block = dynamic_cast<const BlockStmt*>(stmt.cases[i]->body.get())) {
             if (!block->statements.empty()) {
                if (dynamic_cast<const FallthroughStmt*>(block->statements.back().get())) {
                    has_fallthrough = true;
                }
            }
        }

        size_t j = i + 1;
        while (has_fallthrough && j < stmt.cases.size()) {
            current_cases.push_back(stmt.cases[j].get());
            if (const auto* block = dynamic_cast<const BlockStmt*>(stmt.cases[j]->body.get())) {
                if (!block->statements.empty()) {
                    if (!dynamic_cast<const FallthroughStmt*>(block->statements.back().get())) {
                        has_fallthrough = false;
                    }
                } else {
                  has_fallthrough = false;
                }
            }
            j++;
        }

        if (first_case) {
            out << "if (";
            first_case = false;
        } else {
            out << "else if (";
        }

        for (size_t k = 0; k < current_cases.size(); ++k) {
            out << "__switch_val == " << std::any_cast<std::string>(current_cases[k]->value->accept(*this));
            if (k < current_cases.size() - 1) {
                out << " || ";
            }
        }
        out << ") {\n";

        for (const auto& case_to_transpile : current_cases) {
            if (const auto* block = dynamic_cast<const BlockStmt*>(case_to_transpile->body.get())) {
                for(const auto& statement : block->statements) {
                    // Don't transpile the fallthrough statement itself
                    if (!dynamic_cast<const FallthroughStmt*>(statement.get())) {
                        statement->accept(*this);
                    }
                }
            }
        }
        out << "}\n";
        i = j;
    }

    if (default_case) {
        out << "else ";
        default_case->body->accept(*this);
    }

    out << "}\n";
    return nullptr;
}
std::any Transpiler::visitCaseStmt(const CaseStmt& stmt) {
    // This should not be called directly. Logic is handled in visitSwitchStmt.
    return nullptr;
}
std::any Transpiler::visitBreakStmt(const BreakStmt& stmt) {
    out << "break;\n";
    return nullptr;
}
std::any Transpiler::visitFallthroughStmt(const FallthroughStmt& stmt) {
    out << "[[fallthrough]];\n";
    return nullptr;
}

std::string Transpiler::transpileType(const TypeExpr& type) {
    if (auto baseType = dynamic_cast<const BaseTypeExpr*>(&type)) {
        // Simple mapping for now. This will need to be expanded.
        if (baseType->type.lexeme == "int") return "int";
        if (baseType->type.lexeme == "string") return "std::string";
        if (baseType->type.lexeme == "bool") return "bool";
        if (baseType->type.lexeme == "void") return "void";
        return baseType->type.lexeme; // For user-defined types (structs)
    }
    // Handle other types like ArrayTypeExpr, etc. here later.
    return "auto"; // Default fallback
}

std::any Transpiler::visitFunctionStmt(const FunctionStmt& stmt) {
    out << (stmt.return_type ? transpileType(*stmt.return_type) : "void") << " " << stmt.name.lexeme << "(";
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        out << transpileType(*stmt.param_types[i]) << " " << stmt.params[i].lexeme;
        if (i < stmt.params.size() - 1) {
            out << ", ";
        }
    }
    out << ") ";
    stmt.body->accept(*this);
    return nullptr;
}

std::any Transpiler::visitStructStmt(const StructStmt& stmt) {
    out << "struct " << stmt.name.lexeme << " {\n";
    for (const auto& field : stmt.fields) {
        // This reuses the VarStmt logic, but we need to adapt it slightly for fields.
        out << transpileType(*field->type) << " " << field->name.lexeme;
        if (field->initializer) {
            out << " = " << std::any_cast<std::string>(field->initializer->accept(*this));
        }
        out << ";\n";
    }

    for (const auto& method : stmt.methods) {
        // Re-using visitFunctionStmt logic here would be ideal, but for now, a direct implementation.
        out << (method->return_type ? transpileType(*method->return_type) : "void") << " " << method->name.lexeme << "(";
        for (size_t i = 0; i < method->params.size(); ++i) {
            out << transpileType(*method->param_types[i]) << " " << method->params[i].lexeme;
            if (i < method->params.size() - 1) {
                out << ", ";
            }
        }
        out << ") ";
        method->body->accept(*this);
    }
    out << "};\n";
    return nullptr;
}

} // namespace chtholly
