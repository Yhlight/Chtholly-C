#include "Transpiler.h"
#include <sstream>
#include <iostream>
#include <functional>
#include <vector>
#include "../common/Error.h"

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements, bool is_module) {
    // Pre-scan for ImplStmts to populate the map
    for (const auto& stmt : statements) {
        if (auto* impl = dynamic_cast<ImplStmt*>(stmt.get())) {
            impls[impl->structName.lexeme].push_back(impl);
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
        bool has_reflect = false;
        for (const auto& statement : statements) {
            if (auto* exprStmt = dynamic_cast<const ExpressionStmt*>(statement.get())) {
                if (auto* callExpr = dynamic_cast<const CallExpr*>(exprStmt->expression.get())) {
                    if (auto* getExpr = dynamic_cast<const GetExpr*>(callExpr->callee.get())) {
                        if (auto* varExpr = dynamic_cast<const VariableExpr*>(getExpr->object.get())) {
                            if (varExpr->name.lexeme == "reflect" && getExpr->name.lexeme == "get_field_names") {
                                has_reflect = true;
                                break;
                            }
                        }
                    }
                }
            } else if (auto* letStmt = dynamic_cast<const LetStmt*>(statement.get())) {
                if (letStmt->initializer) {
                    if (auto* callExpr = dynamic_cast<const CallExpr*>(letStmt->initializer.get())) {
                        if (auto* getExpr = dynamic_cast<const GetExpr*>(callExpr->callee.get())) {
                            if (auto* varExpr = dynamic_cast<const VariableExpr*>(getExpr->object.get())) {
                                if (varExpr->name.lexeme == "reflect" && getExpr->name.lexeme == "get_field_names") {
                                    has_reflect = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (has_reflect) {
            out << "#include <vector>\n";
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
    if (expr.left->resolved_type) {
        auto type = expr.left->resolved_type;
        if (impls.count(type->baseType.lexeme)) {
            const auto& impl_list = impls.at(type->baseType.lexeme);
            if (op_to_trait.count(expr.op.type)) {
                std::string trait_name = op_to_trait.at(expr.op.type);
                std::string method_name = op_to_method.at(expr.op.type);
                for (const auto& impl : impl_list) {
                    if (impl->traitName && impl->traitName->lexeme == trait_name) {
                        return evaluate(*expr.left) + "." + method_name + "(" + evaluate(*expr.right) + ")";
                    }
                }
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
    } else if (type.baseType.lexeme == "int") {
        cpp_type = "int";
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
    } else if (stmt.initializer && stmt.initializer->resolved_type) {
        out << to_cpp_type(*stmt.initializer->resolved_type) << " " << stmt.name.lexeme;
    } else {
        out << "auto " << stmt.name.lexeme;
    }
    if (stmt.initializer) {
        out << " = " << evaluate(*stmt.initializer);
    }
    out << ";\n";
    return {};
}

std::any Transpiler::visitSwitchStmt(const SwitchStmt& stmt) {
    std::string switch_var = "switch_val";
    out << "    do {\n";
    out << "        auto " << switch_var << " = " << evaluate(*stmt.expression) << ";\n";
    out << "        bool matched = false;\n";

    for (const auto& case_stmt : stmt.cases) {
        out << "        if (!matched && (" << switch_var << " == " << evaluate(*case_stmt.condition) << ")) {\n";
        out << "            matched = true;\n";
        out << "        }\n";
        out << "        if (matched) ";
        execute(*case_stmt.body);
    }

    if (stmt.defaultCase) {
        out << "        if (!matched) ";
        execute(*stmt.defaultCase->body);
    }
    out << "    } while(false);\n";
    return {};
}

std::any Transpiler::visitBreakStmt(const BreakStmt& stmt) {
    out << "    break;\n";
    return {};
}

std::any Transpiler::visitFallthroughStmt(const FallthroughStmt& stmt) {
    // For if-else if, fallthrough is the default behavior.
    return {};
}

std::any Transpiler::visitStructInitializerExpr(const StructInitializerExpr& expr) {
    std::stringstream ss;
    ss << expr.name.lexeme << "{";
    bool first = true;
    for (const auto& [name, value] : expr.initializers) {
        if (!first) {
            ss << ", ";
        }
        ss << "." << name.lexeme << " = " << evaluate(*value);
        first = false;
    }
    ss << "}";
    return ss.str();
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

#include "../driver/Chtholly.h"
#include <fstream>
std::any Transpiler::visitImplStmt(const ImplStmt& stmt) {
    // This is handled in the pre-scan
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

    Transpiler transpiler(resolver);
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
        out << " : ";
        bool first = true;
        for (const auto* impl : impls.at(stmt.name.lexeme)) {
            if (impl->traitName) {
                if (!first) {
                    out << ", ";
                }
                out << "public " << impl->traitName->lexeme;
                first = false;
            }
        }
    }
    out << " {\n";

    if (!stmt.fields.empty()) {
        if (stmt.fields[0]->is_public) {
            out << "public:\n";
        } else {
            out << "private:\n";
        }
    }

    bool in_public = true;
    for (const auto& field : stmt.fields) {
        if (field->is_public && !in_public) {
            out << "public:\n";
            in_public = true;
        } else if (!field->is_public && in_public) {
            out << "private:\n";
            in_public = false;
        }
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
        for (const auto* impl : impls.at(stmt.name.lexeme)) {
            for (const auto& method : impl->methods) {
                visitFunctionStmt(*method, stmt.name);
            }
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

    if (auto* get = dynamic_cast<const GetExpr*>(expr.callee.get())) {
        if (auto* var = dynamic_cast<const VariableExpr*>(get->object.get())) {
            if (var->name.lexeme == "reflect" && get->name.lexeme == "get_field_names") {
                if (expr.generic_args.size() == 1) {
                    const auto& type = expr.generic_args[0];
                    if (resolver.structs.count(type.baseType.lexeme)) {
                        const auto* struct_decl = resolver.structs.at(type.baseType.lexeme);
                        std::string result = "std::vector<std::string>{";
                        for (size_t i = 0; i < struct_decl->fields.size(); ++i) {
                            result += "\"" + struct_decl->fields[i]->name.lexeme + "\"";
                            if (i < struct_decl->fields.size() - 1) {
                                result += ", ";
                            }
                        }
                        result += "}";
                        return result;
                    }
                }
            } else if (var->name.lexeme == "meta") {
                if (expr.generic_args.size() == 1) {
                    const auto& type = expr.generic_args[0];
                    if (get->name.lexeme == "is_struct") {
                        return std::make_any<std::string>(resolver.structs.count(type.baseType.lexeme) ? "true" : "false");
                    } else if (get->name.lexeme == "is_int") {
                        return std::make_any<std::string>(type.baseType.lexeme == "int" ? "true" : "false");
                    } else if (get->name.lexeme == "is_uint") {
                        return std::make_any<std::string>(type.baseType.lexeme == "uint" ? "true" : "false");
                    } else if (get->name.lexeme == "is_double") {
                        return std::make_any<std::string>(type.baseType.lexeme == "double" ? "true" : "false");
                    } else if (get->name.lexeme == "is_char") {
                        return std::make_any<std::string>(type.baseType.lexeme == "char" ? "true" : "false");
                    } else if (get->name.lexeme == "is_bool") {
                        return std::make_any<std::string>(type.baseType.lexeme == "bool" ? "true" : "false");
                    } else if (get->name.lexeme == "is_string") {
                        return std::make_any<std::string>(type.baseType.lexeme == "string" ? "true" : "false");
                    } else if (get->name.lexeme == "is_array") {
                        return std::make_any<std::string>(type.baseType.lexeme == "array" ? "true" : "false");
                    }
                }
            }
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

std::any Transpiler::visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName) {
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
        out << "void";
    }
    out << " " << stmt.name.lexeme << "(";
    bool first = true;
    for (const auto& param : stmt.params) {
        if (param.first.lexeme == "self") continue;
        if (!first) {
            out << ", ";
        }
        out << to_cpp_type(param.second) << " " << param.first.lexeme;
        first = false;
    }
    out << ")";
    if (structName) {
        out << " override";
    }
    out << " {\n";
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
