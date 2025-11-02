#include "Transpiler.h"
#include <sstream>
#include <iostream>
#include <functional>

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    out << "#include <iostream>\n";
    out << "#include <variant>\n";
    bool has_lambda = false;
    for (const auto& statement : statements) {
        if (dynamic_cast<const LetStmt*>(statement.get())) {
            auto* letStmt = dynamic_cast<const LetStmt*>(statement.get());
            if (letStmt->type && letStmt->type->baseType.type == TokenType::FUNCTION) {
                has_lambda = true;
                break;
            }
            if (letStmt->initializer) {
                if (dynamic_cast<const LambdaExpr*>(letStmt->initializer.get())) {
                    has_lambda = true;
                    break;
                }
            }
        }
    }
    if (has_lambda) {
        out << "\n#include <functional>\n\n";
    } else {
        out << "\n";
    }

    for (const auto& statement : statements) {
        if (auto* impl = dynamic_cast<ImplStmt*>(statement.get())) {
            impls[impl->structName.lexeme] = impl;
        }
    }

    for (const auto& statement : statements) {
        if (dynamic_cast<FunctionStmt*>(statement.get()) || dynamic_cast<StructStmt*>(statement.get()) || dynamic_cast<TraitStmt*>(statement.get())) {
            execute(*statement);
        }
    }

    out << "int main() {\n";
    for (const auto& statement : statements) {
        if (!dynamic_cast<FunctionStmt*>(statement.get()) && !dynamic_cast<StructStmt*>(statement.get()) && !dynamic_cast<TraitStmt*>(statement.get()) && !dynamic_cast<ImplStmt*>(statement.get())) {
            execute(*statement);
        }
    }
    out << "    return 0;\n";
    out << "}\n";
    return out.str();
}

void Transpiler::execute(const Stmt& stmt) {
    stmt.accept(*this);
}

std::string Transpiler::evaluate(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::any Transpiler::visitBinaryExpr(const BinaryExpr& expr) {
    return "(" + evaluate(*expr.left) + " " + expr.op.lexeme + " " + evaluate(*expr.right) + ")";
}

std::any Transpiler::visitGroupingExpr(const GroupingExpr& expr) {
    return "(" + evaluate(*expr.expression) + ")";
}

std::any Transpiler::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return "\"" + std::get<std::string>(expr.value) + "\"";
    } else if (std::holds_alternative<double>(expr.value)) {
        std::ostringstream oss;
        oss << std::get<double>(expr.value);
        return oss.str();
    } else if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? std::string("true") : std::string("false");
    }
    return std::string("nullptr");
}

std::any Transpiler::visitUnaryExpr(const UnaryExpr& expr) {
    return "(" + expr.op.lexeme + evaluate(*expr.right) + ")";
}

std::any Transpiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    out << "    " << evaluate(*stmt.expression) << ";\n";
    return {};
}

std::any Transpiler::visitPrintStmt(const PrintStmt& stmt) {
    out << "    std::cout << " << evaluate(*stmt.expression) << " << std::endl;\n";
    return {};
}

std::string to_cpp_type(const TypeInfo& type) {
    std::string cpp_type;
    if (type.baseType.lexeme == "string") {
        cpp_type = "std::string";
    } else if (type.baseType.type == TokenType::FUNCTION) {
        cpp_type = "std::function<";
        if (type.returnType) {
            cpp_type += to_cpp_type(*type.returnType);
        } else {
            cpp_type += "void";
        }
        cpp_type += "(";
        for (size_t i = 0; i < type.params.size(); ++i) {
            cpp_type += to_cpp_type(type.params[i]);
            if (i < type.params.size() - 1) {
                cpp_type += ", ";
            }
        }
        cpp_type += ")>";
    } else {
        cpp_type = type.baseType.lexeme;
    }

    if (type.isReference) {
        if (!type.isMutable) {
            cpp_type = "const " + cpp_type + "&";
        } else {
            cpp_type = cpp_type + "&";
        }
    }
    return cpp_type;
}

std::any Transpiler::visitLetStmt(const LetStmt& stmt) {
    out << "    ";
    bool is_struct = stmt.type && std::isupper(stmt.type->baseType.lexeme[0]);
    if (!stmt.isMutable && !is_struct && !(stmt.type && stmt.type->isReference) && !(stmt.type && stmt.type->baseType.type == TokenType::FUNCTION)) {
        out << "const ";
    }
    if (stmt.type) {
        out << to_cpp_type(*stmt.type) << " " << stmt.name.lexeme;
    } else {
        out << "auto " << stmt.name.lexeme;
    }
    if (stmt.initializer) {
        out << " = " << evaluate(*stmt.initializer);
    }
    out << ";\n";
    return {};
}

std::any Transpiler::visitTraitStmt(const TraitStmt& stmt) {
    out << "struct " << stmt.name.lexeme << " {\n";
    for (const auto& method : stmt.methods) {
        out << "    virtual ";
        if (method->returnType) {
            out << to_cpp_type(*method->returnType);
        } else {
            out << "void";
        }
        out << " " << method->name.lexeme << "(";
        for (size_t i = 0; i < method->params.size(); ++i) {
            out << to_cpp_type(method->params[i].second) << " " << method->params[i].first.lexeme;
            if (i < method->params.size() - 1) {
                out << ", ";
            }
        }
        out << ") = 0;\n";
    }
    out << "};\n\n";
    return {};
}

std::any Transpiler::visitImplStmt(const ImplStmt& stmt) {
    impls[stmt.structName.lexeme] = &stmt;
    return {};
}

std::any Transpiler::visitGetExpr(const GetExpr& expr) {
    return evaluate(*expr.object) + "." + expr.name.lexeme;
}

std::any Transpiler::visitSetExpr(const SetExpr& expr) {
    return evaluate(*expr.object) + "." + expr.name.lexeme + " = " + evaluate(*expr.value);
}

std::any Transpiler::visitBorrowExpr(const BorrowExpr& expr) {
    return "&" + evaluate(*expr.expression);
}

std::any Transpiler::visitLambdaExpr(const LambdaExpr& expr) {
    std::stringstream ss;
    ss << "[](";
    for (size_t i = 0; i < expr.params.size(); ++i) {
        ss << to_cpp_type(expr.params[i].second) << " " << expr.params[i].first.lexeme;
        if (i < expr.params.size() - 1) {
            ss << ", ";
        }
    }
    ss << ") ";
    if (expr.returnType) {
        ss << "-> " << to_cpp_type(*expr.returnType);
    }
    ss << " {\n";
    for (const auto& statement : expr.body) {
        if (auto* returnStmt = dynamic_cast<ReturnStmt*>(statement.get())) {
            if (returnStmt->value) {
                ss << "        return " << evaluate(*returnStmt->value) << ";\n";
            } else {
                ss << "        return;\n";
            }
        }
    }
    ss << "    }";
    return ss.str();
}

std::any Transpiler::visitStructStmt(const StructStmt& stmt) {
    out << "struct " << stmt.name.lexeme;
    if (impls.count(stmt.name.lexeme)) {
        const auto* impl = impls.at(stmt.name.lexeme);
        if (impl->traitName) {
            out << " : public " << impl->traitName->lexeme;
        }
    }
    out << " {\n";

    for (const auto& field : stmt.fields) {
        out << "    ";
        if (field->type) {
            out << to_cpp_type(*field->type) << " " << field->name.lexeme;
        } else {
            out << "auto " << field->name.lexeme;
        }
        if (field->initializer) {
            out << " = " << evaluate(*field->initializer);
        }
        out << ";\n";
    }

    if (impls.count(stmt.name.lexeme)) {
        const auto* impl = impls.at(stmt.name.lexeme);
        for (const auto& method : impl->methods) {
            visitFunctionStmt(*method);
        }
    }
    out << "};\n\n";
    return {};
}

std::any Transpiler::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any Transpiler::visitAssignExpr(const AssignExpr& expr) {
    return expr.name.lexeme + " = " + evaluate(*expr.value);
}

std::any Transpiler::visitBlockStmt(const BlockStmt& stmt) {
    out << "    {\n";
    for (const auto& statement : stmt.statements) {
        execute(*statement);
    }
    out << "    }\n";
    return {};
}

std::any Transpiler::visitIfStmt(const IfStmt& stmt) {
    out << "    if (" << evaluate(*stmt.condition) << ") ";
    execute(*stmt.thenBranch);
    if (stmt.elseBranch) {
        out << " else ";
        execute(*stmt.elseBranch);
    }
    return {};
}

std::any Transpiler::visitWhileStmt(const WhileStmt& stmt) {
    out << "    while (" << evaluate(*stmt.condition) << ") ";
    execute(*stmt.body);
    return {};
}

std::any Transpiler::visitCallExpr(const CallExpr& expr) {
    std::string callee = evaluate(*expr.callee);
    if (!expr.generic_args.empty()) {
        callee += "<";
        for (size_t i = 0; i < expr.generic_args.size(); ++i) {
            callee += to_cpp_type(expr.generic_args[i]);
            if (i < expr.generic_args.size() - 1) {
                callee += ", ";
            }
        }
        callee += ">";
    }
    std::string args;
    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        args += evaluate(*expr.arguments[i]);
        if (i < expr.arguments.size() - 1) {
            args += ", ";
        }
    }
    return callee + "(" + args + ")";
}

std::any Transpiler::visitFunctionStmt(const FunctionStmt& stmt) {
    if (!stmt.generics.empty()) {
        out << "template<";
        for (size_t i = 0; i < stmt.generics.size(); ++i) {
            out << "typename " << stmt.generics[i].lexeme;
            if (i < stmt.generics.size() - 1) {
                out << ", ";
            }
        }
        out << ">\n";
    }
    if (stmt.returnType) {
        out << to_cpp_type(*stmt.returnType);
    } else {
        out << "auto";
    }
    out << " " << stmt.name.lexeme << "(";
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        out << to_cpp_type(stmt.params[i].second) << " " << stmt.params[i].first.lexeme;
        if (i < stmt.params.size() - 1) {
            out << ", ";
        }
    }
    out << ") {\n";
    for (const auto& statement : stmt.body) {
        execute(*statement);
    }
    out << "}\n\n";
    return {};
}

std::any Transpiler::visitReturnStmt(const ReturnStmt& stmt) {
    out << "    return";
    if (stmt.value) {
        if (auto binary = dynamic_cast<const BinaryExpr*>(stmt.value.get())) {
            out << " " << evaluate(*stmt.value).substr(1, evaluate(*stmt.value).length() - 2);
        } else {
            out << " " << evaluate(*stmt.value);
        }
    }
    out << ";\n";
    return {};
}
