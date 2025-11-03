#include "Transpiler.h"
#include <sstream>
#include <iostream>
#include <functional>
#include "Error.h"

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements, bool is_module) {
    symbol_table.emplace_back();
    // Pre-scan for ImplStmts to populate the map
    for (const auto& stmt : statements) {
        if (auto* impl = dynamic_cast<ImplStmt*>(stmt.get())) {
            impls[impl->structName.lexeme] = impl;
        }
    }

    // Phase 1: Header generation for the main file
    if (!is_module) {
        out << "#include <iostream>\n";
        out << "#include <variant>\n";
        out << "#include <string>\n";
        bool has_lambda = false;
        for (const auto& statement : statements) {
            if (auto* letStmt = dynamic_cast<const LetStmt*>(statement.get())) {
                if ((letStmt->type && letStmt->type->baseType.type == TokenType::FUNCTION) ||
                    (letStmt->initializer && dynamic_cast<const LambdaExpr*>(letStmt->initializer.get()))) {
                    has_lambda = true;
                    break;
                }
            }
        }
        if (has_lambda) {
            out << "#include <functional>\n";
        }
        out << "\n";
        out << "std::string chtholly_input() {\n"
            << "    std::string line;\n"
            << "    std::getline(std::cin, line);\n"
            << "    return line;\n"
            << "}\n\n";
        bool has_fs = false;
        for (const auto& statement : statements) {
            if (auto* exprStmt = dynamic_cast<const ExpressionStmt*>(statement.get())) {
                if (auto* callExpr = dynamic_cast<const CallExpr*>(exprStmt->expression.get())) {
                    if (auto* callee = dynamic_cast<const VariableExpr*>(callExpr->callee.get())) {
                        if (callee->name.lexeme == "fs_read" || callee->name.lexeme == "fs_write") {
                            has_fs = true;
                            break;
                        }
                    }
                }
            } else if (auto* letStmt = dynamic_cast<const LetStmt*>(statement.get())) {
                if (letStmt->initializer) {
                    if (auto* callExpr = dynamic_cast<const CallExpr*>(letStmt->initializer.get())) {
                        if (auto* callee = dynamic_cast<const VariableExpr*>(callExpr->callee.get())) {
                            if (callee->name.lexeme == "fs_read" || callee->name.lexeme == "fs_write") {
                                has_fs = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (has_fs) {
            out << "#include <fstream>\n"
                << "#include <sstream>\n\n"
                << "std::string chtholly_fs_read(const std::string& path) {\n"
                << "    std::ifstream file(path);\n"
                << "    if (!file.is_open()) return \"\";\n"
                << "    std::stringstream buffer;\n"
                << "    buffer << file.rdbuf();\n"
                << "    return buffer.str();\n"
                << "}\n\n"
                << "void chtholly_fs_write(const std::string& path, const std::string& content) {\n"
                << "    std::ofstream file(path);\n"
                << "    file << content;\n"
                << "}\n\n";
        }
    }

    // Phase 2: Process imports first
    for (const auto& stmt : statements) {
        if (dynamic_cast<ImportStmt*>(stmt.get())) {
            execute(*stmt);
        }
    }

    // Phase 3: Process global definitions (structs, traits, functions)
    for (const auto& stmt : statements) {
        if (dynamic_cast<StructStmt*>(stmt.get()) ||
            dynamic_cast<TraitStmt*>(stmt.get()) ||
            dynamic_cast<FunctionStmt*>(stmt.get())) {
            execute(*stmt);
        }
    }

    // Phase 4: Process main function body statements if it's not a module
    if (!is_module) {
        out << "int main() {\n";
        for (const auto& stmt : statements) {
            if (!dynamic_cast<StructStmt*>(stmt.get()) &&
                !dynamic_cast<TraitStmt*>(stmt.get()) &&
                !dynamic_cast<FunctionStmt*>(stmt.get()) &&
                !dynamic_cast<ImplStmt*>(stmt.get()) &&
                !dynamic_cast<ImportStmt*>(stmt.get())) {
                execute(*stmt);
            }
        }
        out << "    return 0;\n";
        out << "}\n";
    }

    return out.str();
}

void Transpiler::execute(const Stmt& stmt) {
    stmt.accept(*this);
}

std::string Transpiler::evaluate(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::any Transpiler::visitBinaryExpr(const BinaryExpr& expr) {
    TypeInfo left_type = get_type(*expr.left);
    TypeInfo right_type = get_type(*expr.right);

    if (impls.count(left_type.baseType.lexeme)) {
        std::string trait_name = "operator::" + op_to_trait(expr.op.lexeme);
        const auto* impl = impls.at(left_type.baseType.lexeme);
        for (const auto& name : impl->traitNames) {
            if (name.lexeme == trait_name) {
                return evaluate(*expr.left) + "." + op_to_trait(expr.op.lexeme) + "(" + evaluate(*expr.right) + ")";
            }
        }
    }

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

    TypeInfo type;
    if (stmt.type) {
        type = *stmt.type;
        out << to_cpp_type(type) << " " << stmt.name.lexeme;
    } else {
        type = get_type(*stmt.initializer);
        out << "auto " << stmt.name.lexeme;
    }
    symbol_table.back()[stmt.name.lexeme] = type;

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

#include "Chtholly.h"
#include <fstream>
std::any Transpiler::visitImplStmt(const ImplStmt& stmt) {
    impls[stmt.structName.lexeme] = &stmt;
    return {};
}

std::any Transpiler::visitImportStmt(const ImportStmt& stmt) {
    if (stmt.is_std) {
        return {};
    }
    std::string path = std::get<std::string>(stmt.path.literal);
    if (transpiled_files.count(path)) {
        return {};
    }

    transpiled_files.insert(path);

    std::ifstream file(path);
    if (!file.is_open()) {
        // Error already reported by resolver
        return {};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    Chtholly chtholly;
    auto statements = chtholly.run(buffer.str(), true);

    if (ErrorReporter::hadError) {
        return {};
    }

    Transpiler transpiler;
    // Share the set of already transpiled files to prevent infinite loops
    transpiler.transpiled_files = this->transpiled_files;

    // Transpile the module content and append it to the current output
    out << transpiler.transpile(statements, true);

    // Update the current set of transpiled files
    this->transpiled_files = transpiler.transpiled_files;

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
        if (!impl->traitNames.empty()) {
            out << " : ";
            for (size_t i = 0; i < impl->traitNames.size(); ++i) {
                out << "public " << impl->traitNames[i].lexeme;
                if (i < impl->traitNames.size() - 1) {
                    out << ", ";
                }
            }
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
    symbol_table.emplace_back();
    out << "    {\n";
    for (const auto& statement : stmt.statements) {
        execute(*statement);
    }
    out << "    }\n";
    symbol_table.pop_back();
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
    if (auto* callee_var = dynamic_cast<const VariableExpr*>(expr.callee.get())) {
        if (callee_var->name.lexeme == "print") {
            std::string args;
            for (size_t i = 0; i < expr.arguments.size(); ++i) {
                args += evaluate(*expr.arguments[i]);
                if (i < expr.arguments.size() - 1) {
                    args += " << ";
                }
            }
            return std::make_any<std::string>("std::cout << " + args + " << std::endl");
        } else if (callee_var->name.lexeme == "input") {
            return std::make_any<std::string>("chtholly_input()");
        } else if (callee_var->name.lexeme == "fs_read") {
            std::string args;
            for (size_t i = 0; i < expr.arguments.size(); ++i) {
                args += evaluate(*expr.arguments[i]);
                if (i < expr.arguments.size() - 1) {
                    args += ", ";
                }
            }
            return std::make_any<std::string>("chtholly_fs_read(" + args + ")");
        } else if (callee_var->name.lexeme == "fs_write") {
            std::string args;
            for (size_t i = 0; i < expr.arguments.size(); ++i) {
                args += evaluate(*expr.arguments[i]);
                if (i < expr.arguments.size() - 1) {
                    args += ", ";
                }
            }
            return std::make_any<std::string>("chtholly_fs_write(" + args + ")");
        }
    }

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
    return std::make_any<std::string>(callee + "(" + args + ")");
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

TypeInfo Transpiler::get_type(const Expr& expr) {
    if (auto* literal = dynamic_cast<const LiteralExpr*>(&expr)) {
        if (std::holds_alternative<std::string>(literal->value)) {
            return TypeInfo{Token{TokenType::STRING, "string", "string", 0}};
        } else if (std::holds_alternative<double>(literal->value)) {
            return TypeInfo{Token{TokenType::NUMBER, "double", 0.0, 0}};
        } else if (std::holds_alternative<bool>(literal->value)) {
            return TypeInfo{Token{TokenType::TRUE, "bool", true, 0}};
        }
    } else if (auto* var = dynamic_cast<const VariableExpr*>(&expr)) {
        for (auto it = symbol_table.rbegin(); it != symbol_table.rend(); ++it) {
            if (it->count(var->name.lexeme)) {
                return (*it)[var->name.lexeme];
            }
        }
        ErrorReporter::error(var->name.line, "Undefined variable '" + var->name.lexeme + "'.");
    } else if (auto* binary = dynamic_cast<const BinaryExpr*>(&expr)) {
        TypeInfo left_type = get_type(*binary->left);
        if (impls.count(left_type.baseType.lexeme)) {
            const auto* impl = impls.at(left_type.baseType.lexeme);
            std::string method_name = op_to_trait(binary->op.lexeme);
            for (const auto& method : impl->methods) {
                if (method->name.lexeme == method_name) {
                    return *method->returnType;
                }
            }
        }
    } else if (auto* group = dynamic_cast<const GroupingExpr*>(&expr)) {
        return get_type(*group->expression);
    } else if (auto* call = dynamic_cast<const CallExpr*>(&expr)) {
        // This is a simplified implementation. A real implementation would need to
        // look up the function signature and return its return type.
        if (auto* callee = dynamic_cast<const VariableExpr*>(call->callee.get())) {
            for (auto it = symbol_table.rbegin(); it != symbol_table.rend(); ++it) {
                if (it->count(callee->name.lexeme)) {
                    TypeInfo type = (*it)[callee->name.lexeme];
                    if (type.returnType) {
                        return *type.returnType;
                    }
                }
            }
        }
    }
    return TypeInfo{Token{TokenType::END_OF_FILE, "void", std::monostate{}, 0}};
}
