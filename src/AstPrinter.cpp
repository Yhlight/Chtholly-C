#include "AstPrinter.h"
#include <sstream>

namespace chtholly {

std::string TypePrinter::print(const TypeExpr& type) {
    return std::any_cast<std::string>(type.accept(*this));
}

std::any TypePrinter::visitBaseTypeExpr(const BaseTypeExpr& type) {
    return type.type.lexeme;
}

std::any TypePrinter::visitGenericTypeExpr(const GenericTypeExpr& type) {
    std::stringstream out;
    out << type.base_type.lexeme << "<";
    for (size_t i = 0; i < type.generic_args.size(); ++i) {
        out << print(*type.generic_args[i]);
        if (i < type.generic_args.size() - 1) {
            out << ", ";
        }
    }
    out << ">";
    return out.str();
}

std::any TypePrinter::visitArrayTypeExpr(const ArrayTypeExpr& type) {
    std::stringstream out;
    out << "array<" << print(*type.element_type);
    if (type.size) {
        AstPrinter printer;
        std::map<std::string, const StructStmt*> structs;
        std::map<std::string, const EnumStmt*> enums;
        out << "; " << std::any_cast<std::string>(type.size->accept(printer, structs, enums));
    }
    out << ">";
    return out.str();
}

std::any TypePrinter::visitBorrowTypeExpr(const BorrowTypeExpr& type) {
    return (type.isMutable ? "&mut " : "&") + print(*type.element_type);
}

std::any TypePrinter::visitFunctionTypeExpr(const FunctionTypeExpr& type) {
    std::stringstream out;
    out << "func(";
    for (size_t i = 0; i < type.param_types.size(); ++i) {
        out << print(*type.param_types[i]);
        if (i < type.param_types.size() - 1) {
            out << ", ";
        }
    }
    out << ") -> " << print(*type.return_type);
    return out.str();
}

template<typename... Args>
std::string AstPrinter::parenthesize(const std::string& name, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums, Args... args) {
    std::stringstream out;
    out << "(" << name;
    std::vector<std::string> arg_strings;
    ((arg_strings.push_back(std::any_cast<std::string>(args->accept(*this, structs, enums)))), ...);
    for (const auto& s : arg_strings) {
        out << " " << s;
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitTraitStmt(const TraitStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    out << "(trait " << stmt.name.lexeme;
    for (const auto& method : stmt.methods) {
        out << " " << std::any_cast<std::string>(method->accept(*this, structs, enums));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitArrayLiteralExpr(const ArrayLiteralExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    out << "(array";
    for (const auto& element : expr.elements) {
        out << " " << std::any_cast<std::string>(element->accept(*this, structs, enums));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitTypeCastExpr(const TypeCastExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize("type_cast <type>", structs, enums, expr.expression.get());
}

std::string AstPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    for (const auto& stmt : statements) {
        out << std::any_cast<std::string>(stmt->accept(*this, structs, enums));
    }
    return out.str();
}

std::any AstPrinter::visitBinaryExpr(const BinaryExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize(expr.op.lexeme, structs, enums, expr.left.get(), expr.right.get());
}

std::any AstPrinter::visitUnaryExpr(const UnaryExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize(expr.op.lexeme, structs, enums, expr.right.get());
}

std::any AstPrinter::visitLiteralExpr(const LiteralExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    if (std::holds_alternative<std::nullptr_t>(expr.value)) {
        return std::string("none");
    }
    if (std::holds_alternative<std::string>(expr.value)) {
        return std::get<std::string>(expr.value);
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
        return std::string(1, std::get<char>(expr.value));
    }
    return std::string("unknown literal");
}

std::any AstPrinter::visitGroupingExpr(const GroupingExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize("group", structs, enums, expr.expression.get());
}

std::any AstPrinter::visitVariableExpr(const VariableExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return expr.name.lexeme;
}

std::any AstPrinter::visitAssignExpr(const AssignExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize("= " + expr.name.lexeme, structs, enums, expr.value.get());
}

std::any AstPrinter::visitCallExpr(const CallExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    out << "(call " << std::any_cast<std::string>(expr.callee->accept(*this, structs, enums));
    for (const auto& arg : expr.arguments) {
        out << " " << std::any_cast<std::string>(arg->accept(*this, structs, enums));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitLambdaExpr(const LambdaExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return std::string("(lambda)");
}

std::any AstPrinter::visitGetExpr(const GetExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    auto temp_var = std::make_unique<VariableExpr>(expr.name);
    return parenthesize(".", structs, enums, expr.object.get(), temp_var.get());
}

std::any AstPrinter::visitSetExpr(const SetExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize("= ." + expr.name.lexeme, structs, enums, expr.object.get(), expr.value.get());
}

std::any AstPrinter::visitSelfExpr(const SelfExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return std::string("this");
}

std::any AstPrinter::visitBorrowExpr(const BorrowExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize(expr.isMutable ? "&mut" : "&", structs, enums, expr.expression.get());
}

std::any AstPrinter::visitDerefExpr(const DerefExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize("*", structs, enums, expr.expression.get());
}

std::any AstPrinter::visitStructLiteralExpr(const StructLiteralExpr& expr, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    out << "(struct_literal " << expr.name.lexeme;
    for (const auto& [name, value] : expr.fields) {
        out << " (:= " << name << " " << std::any_cast<std::string>(value->accept(*this, structs, enums)) << ")";
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitBlockStmt(const BlockStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    out << "(block";
    for (const auto& s : stmt.statements) {
        out << " " << std::any_cast<std::string>(s->accept(*this, structs, enums));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitExpressionStmt(const ExpressionStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize(";", structs, enums, stmt.expression.get());
}

std::any AstPrinter::visitFunctionStmt(const FunctionStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    out << "(func " << stmt.name.lexeme;

    if (!stmt.generic_params.empty()) {
        out << " <";
        for (size_t i = 0; i < stmt.generic_params.size(); ++i) {
            out << stmt.generic_params[i].lexeme;
            if (stmt.generic_constraints.count(stmt.generic_params[i].lexeme)) {
                out << " ? ";
                // This part is simplified for brevity. A real implementation
                // would need to print the type expressions of the constraints.
                out << stmt.generic_constraints.at(stmt.generic_params[i].lexeme).size() << "_constraints";
            }
            if (i < stmt.generic_params.size() - 1) {
                out << ", ";
            }
        }
        out << ">";
    }

    if (stmt.body) {
        out << " " << std::any_cast<std::string>(stmt.body->accept(*this, structs, enums));
    }

    out << ")";
    return out.str();
}

std::any AstPrinter::visitIfStmt(const IfStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    if (stmt.elseBranch) {
        return parenthesize("if-else", structs, enums, stmt.condition.get(), stmt.thenBranch.get(), stmt.elseBranch.get());
    }
    return parenthesize("if", structs, enums, stmt.condition.get(), stmt.thenBranch.get());
}

std::any AstPrinter::visitVarStmt(const VarStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    if (stmt.initializer) {
        return parenthesize((stmt.isMutable ? "mut " : "let ") + stmt.name.lexeme, structs, enums, stmt.initializer.get());
    }
    return "(" + std::string(stmt.isMutable ? "mut " : "let ") + stmt.name.lexeme + ")";
}

std::any AstPrinter::visitWhileStmt(const WhileStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize("while", structs, enums, stmt.condition.get(), stmt.body.get());
}

std::any AstPrinter::visitForStmt(const ForStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    out << "(for";
    out << " " << (stmt.initializer ? std::any_cast<std::string>(stmt.initializer->accept(*this, structs, enums)) : "null");
    out << " " << (stmt.condition ? std::any_cast<std::string>(stmt.condition->accept(*this, structs, enums)) : "null");
    out << " " << (stmt.increment ? std::any_cast<std::string>(stmt.increment->accept(*this, structs, enums)) : "null");
    out << " " << std::any_cast<std::string>(stmt.body->accept(*this, structs, enums));
    out << ")";
    return out.str();
}

std::any AstPrinter::visitReturnStmt(const ReturnStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    if (stmt.value) {
        return parenthesize("return", structs, enums, stmt.value.get());
    }
    return std::string("(return)");
}

std::any AstPrinter::visitStructStmt(const StructStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    out << "(struct " << stmt.name.lexeme;
    if (!stmt.traits.empty()) {
        out << " (impl";
        for (const auto& trait : stmt.traits) {
            out << " " << std::any_cast<std::string>(trait->accept(*this, structs, enums));
        }
        out << ")";
    }
    TypePrinter type_printer;
    for (const auto& field : stmt.fields) {
        out << " (field " << field->name.lexeme << ": " << (field->type ? type_printer.print(*field->type) : "auto") << ")";
    }
    for (const auto& method : stmt.methods) {
        out << " " << std::any_cast<std::string>(method->accept(*this, structs, enums));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitImportStmt(const ImportStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    if (std::holds_alternative<std::string>(stmt.path)) {
        auto literal = std::make_unique<LiteralExpr>(std::get<std::string>(stmt.path));
        return parenthesize("import", structs, enums, literal.get());
    }
    auto var = std::make_unique<VariableExpr>(std::get<Token>(stmt.path));
    return parenthesize("import", structs, enums, var.get());
}

std::any AstPrinter::visitSwitchStmt(const SwitchStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    out << "(switch";
    for (const auto& c : stmt.cases) {
        out << " " << std::any_cast<std::string>(c->accept(*this, structs, enums));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitCaseStmt(const CaseStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return parenthesize("case", structs, enums, stmt.value.get(), stmt.body.get());
}

std::any AstPrinter::visitBreakStmt(const BreakStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return std::string("(break)");
}

std::any AstPrinter::visitFallthroughStmt(const FallthroughStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    return std::string("(fallthrough)");
}

std::any AstPrinter::visitEnumStmt(const EnumStmt& stmt, std::map<std::string, const StructStmt*>& structs, std::map<std::string, const EnumStmt*>& enums) {
    std::stringstream out;
    out << "(enum " << stmt.name.lexeme;
    for (const auto& member : stmt.members) {
        out << " " << member.lexeme;
    }
    out << ")";
    return out.str();
}

} // namespace chtholly
