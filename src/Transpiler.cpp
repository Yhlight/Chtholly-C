#include "Transpiler.h"
#include <sstream>
#include <iostream>
#include <functional>
#include "Error.h"

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements, bool is_module) {
    symbol_table.emplace_back();
    // Pre-scan for ImplStmts to populate the map
    for (const auto& stmt : statements) {
        if (auto* imp = dynamic_cast<ImportStmt*>(stmt.get())) {
            if (imp->is_std) {
                imported_std_modules.insert(imp->path.lexeme);
            }
        } else if (auto* s = dynamic_cast<StructStmt*>(stmt.get())) {
            structs[s->name.lexeme] = s;
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
        if (imported_std_modules.count("reflect")) {
            out << "#include <any>\n";
            out << "class chtholly_field {\n"
                << "public:\n"
                << "    chtholly_field(std::string name, std::string type, std::any value)\n"
                << "        : name(std::move(name)), type(std::move(type)), value(std::move(value)) {}\n\n"
                << "    std::string get_name() const { return name; }\n"
                << "    std::string get_type() const { return type; }\n"
                << "    std::any get_value() const { return value; }\n\n"
                << "private:\n"
                << "    std::string name;\n"
                << "    std::string type;\n"
                << "    std::any value;\n"
                << "};\n\n";
            out << "struct chtholly_parameter {\n"
                << "    std::string name;\n"
                << "    std::string type;\n"
                << "};\n\n";
            out << "class chtholly_method {\n"
                << "public:\n"
                << "    chtholly_method(std::string name, std::string return_type, std::vector<chtholly_parameter> parameters)\n"
                << "        : name(std::move(name)), return_type(std::move(return_type)), parameters(std::move(parameters)) {}\n\n"
                << "    std::string get_name() const { return name; }\n"
                << "    std::string get_return_type() const { return return_type; }\n"
                << "    std::vector<chtholly_parameter> get_parameters() const { return parameters; }\n\n"
                << "private:\n"
                << "    std::string name;\n"
                << "    std::string return_type;\n"
                << "    std::vector<chtholly_parameter> parameters;\n"
                << "};\n\n";
        }
        if (imported_std_modules.count("iostream")) {
            out << "\n";
            out << "std::string chtholly_input() {\n"
                << "    std::string line;\n"
                << "    std::getline(std::cin, line);\n"
                << "    return line;\n"
                << "}\n\n";
        }
        if (imported_std_modules.count("filesystem")) {
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

    if (structs.count(left_type.baseType.lexeme)) {
        const auto* s = structs.at(left_type.baseType.lexeme);
        std::string trait_name = "operator::" + op_to_trait(expr.op.lexeme);
        bool found = false;
        for (const auto& name : s->traitNames) {
            if (name.lexeme == trait_name) {
                found = true;
                break;
            }
        }
        if (found) {
            return evaluate(*expr.left) + "." + op_to_trait(expr.op.lexeme) + "(" + evaluate(*expr.right) + ")";
        } else {
            ErrorReporter::error(expr.op.line, "Binary operator '" + expr.op.lexeme + "' is not overloaded for type '" + left_type.baseType.lexeme + "'.");
            return "nullptr";
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
std::any Transpiler::visitImportStmt(const ImportStmt& stmt) {
    if (stmt.is_std) {
        imported_std_modules.insert(stmt.path.lexeme);
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
    if (!stmt.traitNames.empty()) {
        out << " : ";
        for (size_t i = 0; i < stmt.traitNames.size(); ++i) {
            out << "public " << stmt.traitNames[i].lexeme;
            if (i < stmt.traitNames.size() - 1) {
                out << ", ";
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

    for (const auto& method : stmt.methods) {
        out << "    ";
        visitFunctionStmt(*method);
    }
    out << "};\n\n";
    return {};
}

std::any Transpiler::visitVariableExpr(const VariableExpr& expr) {
    if (expr.name.lexeme == "self" && is_in_method) {
        return "this";
    }
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
        if (callee_var->name.lexeme == "print" || callee_var->name.lexeme == "input") {
            if (imported_std_modules.find("iostream") == imported_std_modules.end()) {
                ErrorReporter::error(callee_var->name.line, "Function '" + callee_var->name.lexeme + "' requires 'iostream' module to be imported.");
                return std::make_any<std::string>("");
            }
        } else if (callee_var->name.lexeme == "fs_read" || callee_var->name.lexeme == "fs_write") {
            if (imported_std_modules.find("filesystem") == imported_std_modules.end()) {
                ErrorReporter::error(callee_var->name.line, "Function '" + callee_var->name.lexeme + "' requires 'filesystem' module to be imported.");
                return std::make_any<std::string>("");
            }
        }

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
    } else if (auto* get = dynamic_cast<const GetExpr*>(expr.callee.get())) {
        if (auto* get2 = dynamic_cast<const GetExpr*>(get->object.get())) {
            if (auto* var = dynamic_cast<const VariableExpr*>(get2->object.get())) {
                if (var->name.lexeme == "reflect" && get2->name.lexeme == "field" && get->name.lexeme == "get_fields") {
                    if (expr.arguments.size() != 1) {
                        ErrorReporter::error(expr.paren.line, "get_fields() expects 1 argument.");
                        return std::make_any<std::string>("");
                    }
                    TypeInfo type = get_type(*expr.arguments[0]);
                    if (structs.find(type.baseType.lexeme) == structs.end()) {
                        ErrorReporter::error(expr.paren.line, "get_fields() can only be used on structs.");
                        return std::make_any<std::string>("");
                    }
                    const auto* s = structs.at(type.baseType.lexeme);
                    std::string result = "std::vector<chtholly_field>{\n";
                    for (const auto& field : s->fields) {
                        result += "        chtholly_field(\"" + field->name.lexeme + "\", \"" + field->type->baseType.lexeme + "\", " + evaluate(*expr.arguments[0]) + "." + field->name.lexeme + "),\n";
                    }
                    result += "    }";
                    return std::make_any<std::string>(result);
                } else if (var->name.lexeme == "reflect" && get2->name.lexeme == "field" && get->name.lexeme == "get_field") {
                    if (expr.arguments.size() != 2) {
                        ErrorReporter::error(expr.paren.line, "get_field() expects 2 arguments.");
                        return std::make_any<std::string>("");
                    }
                    TypeInfo type = get_type(*expr.arguments[0]);
                    if (structs.find(type.baseType.lexeme) == structs.end()) {
                        ErrorReporter::error(expr.paren.line, "get_field() can only be used on structs.");
                        return std::make_any<std::string>("");
                    }
                    const auto* s = structs.at(type.baseType.lexeme);
                    std::string field_name = evaluate(*expr.arguments[1]);
                    // remove quotes
                    field_name = field_name.substr(1, field_name.length() - 2);
                    for (const auto& field : s->fields) {
                        if (field->name.lexeme == field_name) {
                            return std::make_any<std::string>("chtholly_field(\"" + field->name.lexeme + "\", \"" + field->type->baseType.lexeme + "\", " + evaluate(*expr.arguments[0]) + "." + field->name.lexeme + ")");
                        }
                    }
                    ErrorReporter::error(expr.paren.line, "Struct '" + type.baseType.lexeme + "' does not have a field named '" + field_name + "'.");
                    return std::make_any<std::string>("");
                }
            } else if (var->name.lexeme == "reflect" && get2->name.lexeme == "method" && get->name.lexeme == "get_methods") {
                if (expr.arguments.size() != 1) {
                    ErrorReporter::error(expr.paren.line, "get_methods() expects 1 argument.");
                    return std::make_any<std::string>("");
                }
                TypeInfo type = get_type(*expr.arguments[0]);
                if (structs.find(type.baseType.lexeme) == structs.end()) {
                    ErrorReporter::error(expr.paren.line, "get_methods() can only be used on structs.");
                    return std::make_any<std::string>("");
                }
                const auto* s = structs.at(type.baseType.lexeme);
                std::string result = "std::vector<chtholly_method>{\n";
                for (const auto& method : s->methods) {
                    result += "        chtholly_method(\"" + method->name.lexeme + "\", \"" + (method->returnType ? method->returnType->baseType.lexeme : "void") + "\", {}),\n";
                }
                result += "    }";
                return std::make_any<std::string>(result);
            } else if (var->name.lexeme == "reflect" && get2->name.lexeme == "method" && get->name.lexeme == "get_method") {
                if (expr.arguments.size() != 2) {
                    ErrorReporter::error(expr.paren.line, "get_method() expects 2 arguments.");
                    return std::make_any<std::string>("");
                }
                TypeInfo type = get_type(*expr.arguments[0]);
                if (structs.find(type.baseType.lexeme) == structs.end()) {
                    ErrorReporter::error(expr.paren.line, "get_method() can only be used on structs.");
                    return std::make_any<std::string>("");
                }
                const auto* s = structs.at(type.baseType.lexeme);
                std::string method_name = evaluate(*expr.arguments[1]);
                // remove quotes
                method_name = method_name.substr(1, method_name.length() - 2);
                for (const auto& method : s->methods) {
                    if (method->name.lexeme == method_name) {
                        return std::make_any<std::string>("chtholly_method(\"" + method->name.lexeme + "\", \"" + (method->returnType ? method->returnType->baseType.lexeme : "void") + "\", {})");
                    }
                }
                ErrorReporter::error(expr.paren.line, "Struct '" + type.baseType.lexeme + "' does not have a method named '" + method_name + "'.");
                return std::make_any<std::string>("");
            } else if (var->name.lexeme == "meta") {
                if (expr.arguments.size() != 1) {
                    ErrorReporter::error(expr.paren.line, "meta functions expect 1 argument.");
                    return std::make_any<std::string>("");
                }
                TypeInfo type = get_type(*expr.arguments[0]);
                if (get2->name.lexeme == "is_int") {
                    return std::make_any<std::string>(type.baseType.lexeme == "int" ? "true" : "false");
                } else if (get2->name.lexeme == "is_string") {
                    return std::make_any<std::string>(type.baseType.lexeme == "string" ? "true" : "false");
                } else if (get2->name.lexeme == "is_struct") {
                    return std::make_any<std::string>(structs.count(type.baseType.lexeme) ? "true" : "false");
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

std::any Transpiler::visitFunctionStmt(const FunctionStmt& stmt) {
    bool previous_is_in_method = is_in_method;
    is_in_method = true;
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
    is_in_method = previous_is_in_method;
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
        if (structs.count(left_type.baseType.lexeme)) {
            const auto* s = structs.at(left_type.baseType.lexeme);
            std::string method_name = op_to_trait(binary->op.lexeme);
            for (const auto& method : s->methods) {
                if (method->name.lexeme == method_name) {
                    return *method->returnType;
                }
            }
            // If we're here, it means the operator is not overloaded for the struct.
            // The error is reported in visitBinaryExpr, so we just return a void type.
            return TypeInfo{Token{TokenType::END_OF_FILE, "void", std::monostate{}, 0}};
        }
        // For non-struct types, we assume the result type is the same as the left operand.
        return left_type;
    } else if (auto* group = dynamic_cast<const GroupingExpr*>(&expr)) {
        return get_type(*group->expression);
    } else if (auto* call = dynamic_cast<const CallExpr*>(&expr)) {
        // This is a simplified implementation. A real implementation would need to
        // look up the function signature and return its return type.
        if (auto* get = dynamic_cast<const GetExpr*>(call->callee.get())) {
            TypeInfo object_type = get_type(*get->object);
            if (structs.count(object_type.baseType.lexeme)) {
                const auto* s = structs.at(object_type.baseType.lexeme);
                for (const auto& method : s->methods) {
                    if (method->name.lexeme == get->name.lexeme) {
                        return *method->returnType;
                    }
                }
            }
        } else if (auto* callee = dynamic_cast<const VariableExpr*>(call->callee.get())) {
            for (auto it = symbol_table.rbegin(); it != symbol_table.rend(); ++it) {
                if (it->count(callee->name.lexeme)) {
                    TypeInfo type = (*it)[callee->name.lexeme];
                    if (type.returnType) {
                        return *type.returnType;
                    }
                }
            }
        }
    } else if (auto* get = dynamic_cast<const GetExpr*>(&expr)) {
        TypeInfo object_type = get_type(*get->object);
        if (structs.count(object_type.baseType.lexeme)) {
            const auto* s = structs.at(object_type.baseType.lexeme);
            for (const auto& field : s->fields) {
                if (field->name.lexeme == get->name.lexeme) {
                    return *field->type;
                }
            }
        }
    }
    return TypeInfo{Token{TokenType::END_OF_FILE, "void", std::monostate{}, 0}};
}
