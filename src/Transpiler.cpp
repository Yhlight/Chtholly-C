#include "Transpiler.h"
#include <sstream>
#include <iostream>
#include <functional>
#include "Error.h"

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements, bool is_module) {
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
        bool has_option = false;
        for (const auto& statement : statements) {
            if (auto* letStmt = dynamic_cast<const LetStmt*>(statement.get())) {
                if ((letStmt->type && letStmt->type->baseType.type == TokenType::FUNCTION) ||
                    (letStmt->initializer && dynamic_cast<const LambdaExpr*>(letStmt->initializer.get()))) {
                    has_lambda = true;
                }
                if (letStmt->type && letStmt->type->baseType.lexeme == "option") {
                    has_option = true;
                }
            }
        }
        if (has_lambda) {
            out << "#include <functional>\n";
        }
        if (has_option) {
            out << "#include <optional>\n";
        }
        out << "\n";
        bool has_input = false;
        for (const auto& statement : statements) {
            if (auto* exprStmt = dynamic_cast<const ExpressionStmt*>(statement.get())) {
                if (auto* callExpr = dynamic_cast<const CallExpr*>(exprStmt->expression.get())) {
                    if (auto* callee = dynamic_cast<const VariableExpr*>(callExpr->callee.get())) {
                        if (callee->name.lexeme == "input") {
                            has_input = true;
                            break;
                        }
                    }
                }
            } else if (auto* letStmt = dynamic_cast<const LetStmt*>(statement.get())) {
                if (letStmt->initializer) {
                    if (auto* callExpr = dynamic_cast<const CallExpr*>(letStmt->initializer.get())) {
                        if (auto* callee = dynamic_cast<const VariableExpr*>(callExpr->callee.get())) {
                            if (callee->name.lexeme == "input") {
                                has_input = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (has_input) {
            out << "std::string chtholly_input() {\n"
                << "    std::string line;\n"
                << "    std::getline(std::cin, line);\n"
                << "    return line;\n"
                << "}\n\n";
        }
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
            stmt->accept(*this);
        }
    }

    // Phase 3: Process global definitions (structs, traits, functions)
    for (const auto& stmt : statements) {
        if (dynamic_cast<StructStmt*>(stmt.get()) ||
            dynamic_cast<TraitStmt*>(stmt.get()) ||
            dynamic_cast<FunctionStmt*>(stmt.get()) ||
            dynamic_cast<EnumStmt*>(stmt.get())) {
            stmt->accept(*this);
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
                !dynamic_cast<ImportStmt*>(stmt.get()) &&
                !dynamic_cast<EnumStmt*>(stmt.get())) {
                stmt->accept(*this);
            }
        }
        out << "    return 0;\n";
        out << "}\n";
    }

    return out.str();
}

std::string Transpiler::evaluate(const Expr& expr) {
    return expr.accept(*this);
}

std::string Transpiler::visitBinaryExpr(const BinaryExpr& expr) {
    return "(" + evaluate(*expr.left) + " " + expr.op.lexeme + " " + evaluate(*expr.right) + ")";
}

std::string Transpiler::visitGroupingExpr(const GroupingExpr& expr) {
    return "(" + evaluate(*expr.expression) + ")";
}

std::string Transpiler::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return "\"" + std::get<std::string>(expr.value) + "\"";
    } else if (std::holds_alternative<double>(expr.value)) {
        std::ostringstream oss;
        oss << std::get<double>(expr.value);
        return oss.str();
    } else if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    } else if (std::holds_alternative<std::monostate>(expr.value)) {
        return "std::nullopt";
    }
    return "nullptr";
}

std::string Transpiler::visitUnaryExpr(const UnaryExpr& expr) {
    return "(" + expr.op.lexeme + evaluate(*expr.right) + ")";
}

std::string Transpiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    out << "    " << evaluate(*stmt.expression) << ";\n";
    return "";
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
    } else if (type.baseType.lexeme == "option") {
        cpp_type = "std::optional<";
        if (!type.params.empty()) {
            cpp_type += to_cpp_type(type.params[0]);
        }
        cpp_type += ">";
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

std::string Transpiler::visitLetStmt(const LetStmt& stmt) {
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
    return "";
}

std::string Transpiler::visitTraitStmt(const TraitStmt& stmt) {
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
    return "";
}

#include "Chtholly.h"
#include <fstream>
std::string Transpiler::visitImplStmt(const ImplStmt& stmt) {
    // This is handled by visitStructStmt
    return "";
}

std::string Transpiler::visitImportStmt(const ImportStmt& stmt) {
    if (stmt.is_std) {
        return "";
    }
    std::string path = std::get<std::string>(stmt.path.literal);
    if (transpiled_files.count(path)) {
        return "";
    }

    transpiled_files.insert(path);

    std::ifstream file(path);
    if (!file.is_open()) {
        // Error already reported by resolver
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    Chtholly chtholly;
    auto statements = chtholly.run(buffer.str(), true);

    if (ErrorReporter::hadError) {
        return "";
    }

    Transpiler transpiler;
    // Share the set of already transpiled files to prevent infinite loops
    transpiler.transpiled_files = this->transpiled_files;

    // Transpile the module content and append it to the current output
    out << transpiler.transpile(statements, true);

    // Update the current set of transpiled files
    this->transpiled_files = transpiler.transpiled_files;

    return "";
}

std::string Transpiler::visitGetExpr(const GetExpr& expr) {
    if (auto* var = dynamic_cast<const VariableExpr*>(expr.object.get())) {
        if (isupper(var->name.lexeme[0])) {
            return evaluate(*expr.object) + "::" + expr.name.lexeme;
        }
    }
    return evaluate(*expr.object) + "." + expr.name.lexeme;
}

std::string Transpiler::visitSetExpr(const SetExpr& expr) {
    return evaluate(*expr.object) + "." + expr.name.lexeme + " = " + evaluate(*expr.value);
}

std::string Transpiler::visitBorrowExpr(const BorrowExpr& expr) {
    return "&" + evaluate(*expr.expression);
}

std::string Transpiler::visitLambdaExpr(const LambdaExpr& expr) {
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

std::string Transpiler::visitStructStmt(const StructStmt& stmt) {
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
            method->accept(*this);
        }
    }
    out << "};\n\n";
    return "";
}

std::string Transpiler::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::string Transpiler::visitAssignExpr(const AssignExpr& expr) {
    return expr.name.lexeme + " = " + evaluate(*expr.value);
}

std::string Transpiler::visitBlockStmt(const BlockStmt& stmt) {
    out << "    {\n";
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    out << "    }\n";
    return "";
}

std::string Transpiler::visitIfStmt(const IfStmt& stmt) {
    out << "    if (" << evaluate(*stmt.condition) << ") ";
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        out << " else ";
        stmt.elseBranch->accept(*this);
    }
    return "";
}

std::string Transpiler::visitWhileStmt(const WhileStmt& stmt) {
    out << "    while (" << evaluate(*stmt.condition) << ") ";
    stmt.body->accept(*this);
    return "";
}

std::string Transpiler::visitSwitchStmt(const SwitchStmt& stmt) {
    std::string switch_var = "switch_val";
    out << "    do {\n";
    out << "        auto " << switch_var << " = " << evaluate(*stmt.expression) << ";\n";
    out << "        bool matched = false;\n";

    for (const auto& case_stmt : stmt.cases) {
        out << "        if (!matched && (" << switch_var << " == " << evaluate(*case_stmt.condition) << ")) {\n";
        out << "            matched = true;\n";
        out << "        }\n";
        out << "        if (matched) ";
        case_stmt.body->accept(*this);
    }

    if (stmt.defaultCase) {
        out << "        if (!matched) ";
        stmt.defaultCase->body->accept(*this);
    }
    out << "    } while(false);\n";
    return "";
}

std::string Transpiler::visitBreakStmt(const BreakStmt& stmt) {
    out << "    break;\n";
    return "";
}

std::string Transpiler::visitFallthroughStmt(const FallthroughStmt& stmt) {
    // For if-else if, fallthrough is the default behavior.
    return "";
}

std::string Transpiler::visitStructInitializerExpr(const StructInitializerExpr& expr) {
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

std::string Transpiler::visitCallExpr(const CallExpr& expr) {
    if (auto* getExpr = dynamic_cast<const GetExpr*>(expr.callee.get())) {
        if (getExpr->name.lexeme == "unwarp") {
            return evaluate(*getExpr->object) + ".value()";
        }
        if (getExpr->name.lexeme == "unwarp_or") {
            std::string args;
            for (size_t i = 0; i < expr.arguments.size(); ++i) {
                args += evaluate(*expr.arguments[i]);
                if (i < expr.arguments.size() - 1) {
                    args += ", ";
                }
            }
            return evaluate(*getExpr->object) + ".value_or(" + args + ")";
        }
    }

    if (auto* callee_var = dynamic_cast<const VariableExpr*>(expr.callee.get())) {
        if (callee_var->name.lexeme == "print") {
            std::string args;
            for (size_t i = 0; i < expr.arguments.size(); ++i) {
                args += evaluate(*expr.arguments[i]);
                if (i < expr.arguments.size() - 1) {
                    args += " << ";
                }
            }
            return "std::cout << " + args + " << std::endl";
        } else if (callee_var->name.lexeme == "input") {
            return "chtholly_input()";
        } else if (callee_var->name.lexeme == "fs_read") {
            std::string args;
            for (size_t i = 0; i < expr.arguments.size(); ++i) {
                args += evaluate(*expr.arguments[i]);
                if (i < expr.arguments.size() - 1) {
                    args += ", ";
                }
            }
            return "chtholly_fs_read(" + args + ")";
        } else if (callee_var->name.lexeme == "fs_write") {
            std::string args;
            for (size_t i = 0; i < expr.arguments.size(); ++i) {
                args += evaluate(*expr.arguments[i]);
                if (i < expr.arguments.size() - 1) {
                    args += ", ";
                }
            }
            return "chtholly_fs_write(" + args + ")";
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
    return callee + "(" + args + ")";
}

std::string Transpiler::visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName) {
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
    out << " ";
    if (structName) {
        out << structName->lexeme << "::";
    }
    out << stmt.name.lexeme << "(";
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        out << to_cpp_type(stmt.params[i].second) << " " << stmt.params[i].first.lexeme;
        if (i < stmt.params.size() - 1) {
            out << ", ";
        }
    }
    out << ") {\n";
    for (const auto& statement : stmt.body) {
        statement->accept(*this);
    }
    out << "}\n\n";
    return "";
}

std::string Transpiler::visitReturnStmt(const ReturnStmt& stmt) {
    out << "    return";
    if (stmt.value) {
        if (auto binary = dynamic_cast<const BinaryExpr*>(stmt.value.get())) {
            out << " " << evaluate(*stmt.value).substr(1, evaluate(*stmt.value).length() - 2);
        } else {
            out << " " << evaluate(*stmt.value);
        }
    }
    out << ";\n";
    return "";
}

std::string Transpiler::visitEnumStmt(const EnumStmt& stmt) {
    out << "enum class " << stmt.name.lexeme << " {\n";
    for (size_t i = 0; i < stmt.members.size(); ++i) {
        out << "    " << stmt.members[i].lexeme;
        if (i < stmt.members.size() - 1) {
            out << ",";
        }
        out << "\n";
    }
    out << "};\n\n";
    return "";
}
