#include "CodeGen.h"
#include "AST.h"
#include <set>
#include <vector>
#include <array>
#include <utility>

namespace chtholly {

std::string CodeGen::generate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }

    std::string preamble = "#include <string>\n#include <vector>\n#include <array>\n";
    if (hasMoves) {
        preamble += "#include <utility>\n";
    }

    return preamble + out.str();
}

std::string CodeGen::generateVarDecl(const VarDeclStmt& stmt) {
    std::stringstream temp_out;
    if (stmt.keyword.type == TokenType::LET && !dynamic_cast<const ReferenceType*>(stmt.resolvedType.get())) {
        temp_out << "const ";
    }

    if (stmt.resolvedType) {
        temp_out << typeToString(*stmt.resolvedType) << " " << stmt.name.lexeme;
    } else {
        temp_out << "auto " << stmt.name.lexeme;
    }

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
        out << "(" << stmt.increment->accept(*this) << ")";
    }
    out << ") ";
    stmt.body->accept(*this);
}

void CodeGen::visit(const FuncDeclStmt& stmt) {
    out << "auto " << stmt.name.lexeme << "(";
    if (stmt.resolvedSignature) {
        for (size_t i = 0; i < stmt.params.size(); ++i) {
            out << typeToString(*stmt.resolvedSignature->getParamTypes()[i]) << " " << stmt.params[i].name.lexeme;
            if (i < stmt.params.size() - 1) {
                out << ", ";
            }
        }
    }
    out << ")";
    if (stmt.returnType && stmt.resolvedSignature) {
        out << " -> " << typeToString(*stmt.resolvedSignature->getReturnType());
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
    if (expr.op.type == TokenType::EQUAL || expr.op.type == TokenType::PLUS_EQUAL ||
        expr.op.type == TokenType::MINUS_EQUAL || expr.op.type == TokenType::STAR_EQUAL ||
        expr.op.type == TokenType::SLASH_EQUAL || expr.op.type == TokenType::PERCENT_EQUAL) {
        return expr.left->accept(*this) + " " + expr.op.lexeme + " " + expr.right->accept(*this);
    }
    return "(" + expr.left->accept(*this) + " " + expr.op.lexeme + " " + expr.right->accept(*this) + ")";
}

std::string CodeGen::visit(const VariableExpr& expr) {
    if (expr.isMove) {
        hasMoves = true;
        return "std::move(" + expr.name.lexeme + ")";
    }
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

void CodeGen::visit(const EnumDeclStmt& stmt) {
    out << "enum class " << stmt.name.lexeme << " {\n";
    for (size_t i = 0; i < stmt.members.size(); ++i) {
        out << stmt.members[i].lexeme;
        if (i < stmt.members.size() - 1) {
            out << ",\n";
        }
    }
    out << "\n};\n";
}

std::string CodeGen::visit(const ScopedAccessExpr& expr) {
    return expr.scope->accept(*this) + "::" + expr.name.lexeme;
}

std::string CodeGen::visit(const ArrayLiteralExpr& expr) {
    std::stringstream ss;
    ss << "{";
    for (size_t i = 0; i < expr.elements.size(); ++i) {
        ss << expr.elements[i]->accept(*this);
        if (i < expr.elements.size() - 1) {
            ss << ", ";
        }
    }
    ss << "}";
    return ss.str();
}

std::string CodeGen::visit(const SubscriptExpr& expr) {
    return expr.array->accept(*this) + "[" + expr.index->accept(*this) + "]";
}

std::string CodeGen::visit(const BorrowExpr& expr) {
    return "&(" + expr.expression->accept(*this) + ")";
}

std::string CodeGen::visit(const DerefExpr& expr) {
    return "*(" + expr.expression->accept(*this) + ")";
}

std::string CodeGen::typeToString(const Type& type) {
    if (dynamic_cast<const IntType*>(&type)) return "int";
    if (dynamic_cast<const StringType*>(&type)) return "std::string";
    if (dynamic_cast<const BoolType*>(&type)) return "bool";
    if (dynamic_cast<const VoidType*>(&type)) return "void";
    if (auto* enumType = dynamic_cast<const EnumType*>(&type)) return enumType->getName();
    if (auto* arrayType = dynamic_cast<const ArrayType*>(&type)) {
        if (arrayType->getSize().has_value()) {
            return "std::array<" + typeToString(*arrayType->getElementType()) + ", " + std::to_string(arrayType->getSize().value()) + ">";
        } else {
            return "std::vector<" + typeToString(*arrayType->getElementType()) + ">";
        }
    }
    if (auto* refType = dynamic_cast<const ReferenceType*>(&type)) {
        if (refType->getIsMutable()) {
            return typeToString(*refType->getReferencedType()) + "*";
        } else {
            return "const " + typeToString(*refType->getReferencedType()) + "*";
        }
    }
    return "auto";
}

} // namespace chtholly
