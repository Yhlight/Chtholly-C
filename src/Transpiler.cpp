#include "Transpiler.h"
#include <utility>
#include <stdexcept>
#include <map>
#include "Token.h"

namespace chtholly {

// A simple visitor to find all struct declarations in the AST.
class StructScanner : public StmtVisitor {
public:
    std::any visitStructStmt(const StructStmt& stmt) override {
        structs[stmt.name.lexeme] = &stmt;
        return nullptr;
    }
    // We only care about structs, so other visit methods are empty.
    std::any visitBlockStmt(const BlockStmt& stmt) override {
        for (const auto& s : stmt.statements) {
            s->accept(*this);
        }
        return nullptr;
    }
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override { return nullptr; }
    std::any visitFunctionStmt(const FunctionStmt& stmt) override { return nullptr; }
    std::any visitIfStmt(const IfStmt& stmt) override { return nullptr; }
    std::any visitVarStmt(const VarStmt& stmt) override { return nullptr; }
    std::any visitWhileStmt(const WhileStmt& stmt) override { return nullptr; }
    std::any visitReturnStmt(const ReturnStmt& stmt) override { return nullptr; }
    std::any visitImportStmt(const ImportStmt& stmt) override { return nullptr; }
    std::any visitSwitchStmt(const SwitchStmt& stmt) override { return nullptr; }
    std::any visitCaseStmt(const CaseStmt& stmt) override { return nullptr; }
    std::any visitBreakStmt(const BreakStmt& stmt) override { return nullptr; }
    std::any visitFallthroughStmt(const FallthroughStmt& stmt) override { return nullptr; }

    std::map<std::string, const StructStmt*> structs;
};

Transpiler::Transpiler() {
    enterScope(); // Global scope
}

void Transpiler::enterScope() {
    scopes.emplace_back();
}

void Transpiler::exitScope() {
    scopes.pop_back();
}

void Transpiler::define(const std::string& name, const TypeInfo& type) {
    if (scopes.empty()) {
        enterScope();
    }
    scopes.back()[name] = type;
}

TypeInfo Transpiler::lookup(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }
    // In a real compiler, we'd throw a semantic error here.
    // In a real compiler, we'd throw a semantic error here.
    // For now, return a default TypeInfo.
    return TypeInfo{};
}

TypeInfo Transpiler::get_type(const Expr& expr) {
    if (auto literal_expr = dynamic_cast<const LiteralExpr*>(&expr)) {
        if (std::holds_alternative<long long>(literal_expr->value)) return TypeInfo{"int"};
        if (std::holds_alternative<double>(literal_expr->value)) return TypeInfo{"double"};
        if (std::holds_alternative<std::string>(literal_expr->value)) return TypeInfo{"std::string"};
        if (std::holds_alternative<bool>(literal_expr->value)) return TypeInfo{"bool"};
        if (std::holds_alternative<char>(literal_expr->value)) return TypeInfo{"char"};
    }
    if (auto var_expr = dynamic_cast<const VariableExpr*>(&expr)) {
        return lookup(var_expr->name.lexeme);
    }
    if (auto struct_literal = dynamic_cast<const StructLiteralExpr*>(&expr)) {
        return TypeInfo{struct_literal->name.lexeme};
    }
    if (auto binary_expr = dynamic_cast<const BinaryExpr*>(&expr)) {
        static const std::map<TokenType, std::string> binary_op_traits = {
            {TokenType::PLUS, "add"}, {TokenType::MINUS, "sub"}, {TokenType::STAR, "mul"},
            {TokenType::SLASH, "div"}, {TokenType::PERCENT, "mod"}, {TokenType::EQUAL_EQUAL, "assign"},
            {TokenType::BANG_EQUAL, "not_equal"}, {TokenType::LESS, "less"}, {TokenType::LESS_EQUAL, "less_equal"},
            {TokenType::GREATER, "greater"}, {TokenType::GREATER_EQUAL, "greater_equal"},
            {TokenType::AND, "and"}, {TokenType::OR, "or"}
        };
        auto it = binary_op_traits.find(binary_expr->op.type);
        if (it != binary_op_traits.end()) {
            TypeInfo left_type = get_type(*binary_expr->left);
            if (has_trait(left_type.name, it->second)) {
                const StructStmt* s = structs[left_type.name];
                for (const auto& method : s->methods) {
                    if (method->name.lexeme == it->second) {
                        return typeExprToTypeInfo(method->return_type.get());
                    }
                }
            }
        }
    }
    if (auto call_expr = dynamic_cast<const CallExpr*>(&expr)) {
        if (auto var_expr = dynamic_cast<const VariableExpr*>(call_expr->callee.get())) {
            if (var_expr->name.lexeme == "input") {
                return TypeInfo{"std::string"};
            }
        }
        if (auto get_expr = dynamic_cast<const GetExpr*>(call_expr->callee.get())) {
             if (auto var_expr = dynamic_cast<const VariableExpr*>(get_expr->object.get())) {
                if (var_expr->name.lexeme == "meta") {
                    return TypeInfo{"bool"};
                }
            }
            TypeInfo callee_type = get_type(*get_expr->object);
            if (structs.count(callee_type.name)) {
                const StructStmt* s = structs[callee_type.name];
                for (const auto& method : s->methods) {
                    if (method->name.lexeme == get_expr->name.lexeme) {
                        return typeExprToTypeInfo(method->return_type.get());
                    }
                }
            }
        }
    }
    return TypeInfo{"auto"};
}

TypeInfo Transpiler::typeExprToTypeInfo(const TypeExpr* type) {
    if (!type) {
        return TypeInfo{ "auto" };
    }

    if (auto baseType = dynamic_cast<const BaseTypeExpr*>(type)) {
        return TypeInfo{ transpileType(*baseType) };
    }
    if (auto borrowType = dynamic_cast<const BorrowTypeExpr*>(type)) {
        return TypeInfo{ transpileType(*borrowType) };
    }

    // Fallback for more complex types not yet handled.
    return TypeInfo{ "auto" };
}

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    // Pre-scan for struct definitions.
    StructScanner scanner;
    for (const auto& statement : statements) {
        statement->accept(scanner);
    }
    this->structs = scanner.structs;

    for (const auto& statement : statements) {
        statement->accept(*this);
    }

    std::stringstream final_code;
    if (imported_modules.count("iostream")) {
        final_code << "#include <iostream>\n";
        if (input_used) {
            final_code << "#include <string>\n";
            final_code << R"(
std::string input() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}
)";
        }
    }
    if (imported_modules.count("filesystem")) {
        final_code << "#include <fstream>\n";
        final_code << "#include <sstream>\n";
        final_code << R"(
std::string fs_read(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
)";
    }
    // Add other includes here as needed

    final_code << out.str();
    return final_code.str();
}

// Helper function to check if a struct implements a specific trait.
bool Transpiler::has_trait(const std::string& struct_name, const std::string& trait_name) {
    if (structs.count(struct_name)) {
        const StructStmt* s = structs[struct_name];
        for (const auto& trait : s->traits) {
            if (auto get_expr = dynamic_cast<const GetExpr*>(trait.get())) {
                if (auto var_expr = dynamic_cast<const VariableExpr*>(get_expr->object.get())) {
                    if (var_expr->name.lexeme == "operator" && get_expr->name.lexeme == trait_name) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

std::any Transpiler::visitBinaryExpr(const BinaryExpr& expr) {
    static const std::map<TokenType, std::string> binary_op_traits = {
        {TokenType::PLUS, "add"}, {TokenType::MINUS, "sub"}, {TokenType::STAR, "mul"},
        {TokenType::SLASH, "div"}, {TokenType::PERCENT, "mod"}, {TokenType::EQUAL_EQUAL, "assign"},
        {TokenType::BANG_EQUAL, "not_equal"}, {TokenType::LESS, "less"}, {TokenType::LESS_EQUAL, "less_equal"},
        {TokenType::GREATER, "greater"}, {TokenType::GREATER_EQUAL, "greater_equal"},
        {TokenType::AND, "and"}, {TokenType::OR, "or"}
    };

    auto it = binary_op_traits.find(expr.op.type);
    if (it != binary_op_traits.end()) {
        TypeInfo left_type = get_type(*expr.left);
        if (has_trait(left_type.name, it->second)) {
            return std::any_cast<std::string>(expr.left->accept(*this)) + "." + it->second + "(" + std::any_cast<std::string>(expr.right->accept(*this)) + ")";
        }
    }

    return std::any_cast<std::string>(expr.left->accept(*this)) + " " + expr.op.lexeme + " " + std::any_cast<std::string>(expr.right->accept(*this));
}

std::any Transpiler::visitUnaryExpr(const UnaryExpr& expr) {
    static const std::map<TokenType, std::string> unary_op_traits = {
        {TokenType::MINUS, "prefix_sub"}, // Assuming '-' is prefix decrement for overloading
        {TokenType::BANG, "not"}
    };

    auto it = unary_op_traits.find(expr.op.type);
    if (it != unary_op_traits.end()) {
        TypeInfo right_type = get_type(*expr.right);
        if (has_trait(right_type.name, it->second)) {
            return std::any_cast<std::string>(expr.right->accept(*this)) + "." + it->second + "()";
        }
    }

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
        return std::string(std::get<bool>(expr.value) ? "true" : "false");
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
std::any Transpiler::handleMetaFunction(const CallExpr& expr) {
    auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get());
    std::string function_name = get_expr->name.lexeme;

    if (expr.arguments.empty()) {
        return std::string("/* ERROR: meta function requires one argument */");
    }
    TypeInfo arg_type = get_type(*expr.arguments[0]);

    if (function_name == "is_int") {
        return std::string(arg_type.name == "int" ? "true" : "false");
    }
    if (function_name == "is_uint") {
        return std::string(arg_type.name == "unsigned int" ? "true" : "false");
    }
    if (function_name == "is_double") {
        return std::string(arg_type.name == "double" ? "true" : "false");
    }
    if (function_name == "is_char") {
        return std::string(arg_type.name == "char" ? "true" : "false");
    }
    if (function_name == "is_bool") {
        return std::string(arg_type.name == "bool" ? "true" : "false");
    }
    if (function_name == "is_string") {
        return std::string(arg_type.name == "std::string" ? "true" : "false");
    }
    if (function_name == "is_struct") {
        return std::string(structs.count(arg_type.name) ? "true" : "false");
    }

    return std::string("/* ERROR: Unknown meta function call */");
}

std::any Transpiler::visitCallExpr(const CallExpr& expr) {
    if (auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get())) {
        if (auto var_expr = dynamic_cast<const VariableExpr*>(get_expr->object.get())) {
            if (var_expr->name.lexeme == "meta") {
                return handleMetaFunction(expr);
            }
        }
    }

    if (auto var_expr = dynamic_cast<const VariableExpr*>(expr.callee.get())) {
        if (var_expr->name.lexeme == "print") {
            if (imported_modules.find("iostream") == imported_modules.end()) {
                return std::string("/* ERROR: 'print' function called without importing 'iostream' */");
            }
            std::stringstream out;
            out << "std::cout << ";
            for (size_t i = 0; i < expr.arguments.size(); ++i) {
                out << std::any_cast<std::string>(expr.arguments[i]->accept(*this));
                if (i < expr.arguments.size() - 1) {
                    out << " << ";
                }
            }
            out << " << std::endl";
            return out.str();
        }
        if (var_expr->name.lexeme == "input") {
            if (imported_modules.find("iostream") == imported_modules.end()) {
                return std::string("/* ERROR: 'input' function called without importing 'iostream' */");
            }
            input_used = true;
            return std::string("input()");
        }
        if (var_expr->name.lexeme == "fs_read") {
            if (imported_modules.find("filesystem") == imported_modules.end()) {
                return std::string("/* ERROR: 'fs_read' function called without importing 'filesystem' */");
            }
            // Simplified: assumes one argument, the filename.
            return "fs_read(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
        }
        if (var_expr->name.lexeme == "fs_write") {
            if (imported_modules.find("filesystem") == imported_modules.end()) {
                return std::string("/* ERROR: 'fs_write' function called without importing 'filesystem' */");
            }
            // Simplified: assumes two arguments, filename and content.
            std::stringstream out;
            out << "{ std::ofstream file(" << std::any_cast<std::string>(expr.arguments[0]->accept(*this)) << "); file << " << std::any_cast<std::string>(expr.arguments[1]->accept(*this)) << "; }";
            return out.str();
        }
    }

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
std::any Transpiler::visitGetExpr(const GetExpr& expr) {
    std::string separator = ".";
    if (dynamic_cast<const SelfExpr*>(expr.object.get())) {
        separator = "->";
    }
    return std::any_cast<std::string>(expr.object->accept(*this)) + separator + expr.name.lexeme;
}
std::any Transpiler::visitSetExpr(const SetExpr& expr) {
    if (expr.name.type == TokenType::STAR) {
        return "*" + std::any_cast<std::string>(expr.object->accept(*this)) + " = " + std::any_cast<std::string>(expr.value->accept(*this));
    }
    return std::any_cast<std::string>(expr.object->accept(*this)) + "." + expr.name.lexeme + " = " + std::any_cast<std::string>(expr.value->accept(*this));
}
std::any Transpiler::visitSelfExpr(const SelfExpr& expr) {
    return std::string("this");
}
std::any Transpiler::visitBorrowExpr(const BorrowExpr& expr) { return nullptr; }
std::any Transpiler::visitDerefExpr(const DerefExpr& expr) {
    return "*" + std::any_cast<std::string>(expr.expression->accept(*this));
}

std::any Transpiler::visitStructLiteralExpr(const StructLiteralExpr& expr) {
    std::stringstream out;
    out << expr.name.lexeme << "{";
    bool first = true;
    for (const auto& [name, value] : expr.fields) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << "." << name << " = " << std::any_cast<std::string>(value->accept(*this));
    }
    out << "}";
    return out.str();
}

std::any Transpiler::visitBlockStmt(const BlockStmt& stmt) {
    visitBlockStmt(stmt, true);
    return nullptr;
}

void Transpiler::visitBlockStmt(const BlockStmt& stmt, bool create_scope) {
    if (create_scope) enterScope();
    out << "{\n";
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    out << "}\n";
    if (create_scope) exitScope();
}

std::any Transpiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    std::string s = std::any_cast<std::string>(stmt.expression->accept(*this));
    out << s << ";\n";
    return nullptr;
}

std::any Transpiler::visitVarStmt(const VarStmt& stmt) {
    TypeInfo type = typeExprToTypeInfo(stmt.type.get());
    if (stmt.initializer && type.name == "auto") {
        type = get_type(*stmt.initializer);
    }

    define(stmt.name.lexeme, type);

    out << (stmt.isMutable ? "" : "const ") << type.name << " " << stmt.name.lexeme;
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
std::any Transpiler::visitImportStmt(const ImportStmt& stmt) {
    if (std::holds_alternative<Token>(stmt.path)) {
        std::string module_name = std::get<Token>(stmt.path).lexeme;
        imported_modules.insert(module_name);
    }
    // File imports are not handled yet.
    return nullptr;
}
std::any Transpiler::visitSwitchStmt(const SwitchStmt& stmt) {
    is_in_switch = true;
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
    is_in_switch = false;
    return nullptr;
}
std::any Transpiler::visitCaseStmt(const CaseStmt& stmt) {
    // This should not be called directly. Logic is handled in visitSwitchStmt.
    return nullptr;
}
std::any Transpiler::visitBreakStmt(const BreakStmt& stmt) {
    if (!is_in_switch) {
        out << "break;\n";
    }
    return nullptr;
}
std::any Transpiler::visitFallthroughStmt(const FallthroughStmt& stmt) {
    out << "[[fallthrough]];\n";
    return nullptr;
}

std::string Transpiler::transpileType(const TypeExpr& type) {
    if (auto baseType = dynamic_cast<const BaseTypeExpr*>(&type)) {
        if (baseType->type.lexeme == "int") return "int";
        if (baseType->type.lexeme == "uint") return "unsigned int";
        if (baseType->type.lexeme == "string") return "std::string";
        if (baseType->type.lexeme == "bool") return "bool";
        if (baseType->type.lexeme == "void") return "void";
        return baseType->type.lexeme;
    }
    if (auto genericType = dynamic_cast<const GenericTypeExpr*>(&type)) {
        std::string s = genericType->base_type.lexeme + "<";
        for (size_t i = 0; i < genericType->generic_args.size(); ++i) {
            s += transpileType(*genericType->generic_args[i]);
            if (i < genericType->generic_args.size() - 1) {
                s += ", ";
            }
        }
        s += ">";
        return s;
    }
    if (auto borrowType = dynamic_cast<const BorrowTypeExpr*>(&type)) {
        return transpileType(*borrowType->element_type) + (borrowType->isMutable ? "&" : " const&");
    }
    return "auto";
}

std::any Transpiler::visitFunctionStmt(const FunctionStmt& stmt) {
    if (!stmt.generic_params.empty()) {
        out << "template <";
        for (size_t i = 0; i < stmt.generic_params.size(); ++i) {
            out << "typename " << stmt.generic_params[i].lexeme;
            if (i < stmt.generic_params.size() - 1) {
                out << ", ";
            }
        }
        out << ">\n";
    }

    out << (stmt.return_type ? transpileType(*stmt.return_type) : "void") << " " << stmt.name.lexeme << "(";

    enterScope();
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        TypeInfo paramType = typeExprToTypeInfo(stmt.param_types[i].get());
        define(stmt.params[i].lexeme, paramType);
        out << paramType.name << " " << stmt.params[i].lexeme;
        if (i < stmt.params.size() - 1) {
            out << ", ";
        }
    }
    out << ") ";

    visitBlockStmt(*stmt.body, false);

    exitScope();
    return nullptr;
}

std::any Transpiler::visitStructStmt(const StructStmt& stmt) {
    if (!stmt.generic_params.empty()) {
        out << "template <";
        for (size_t i = 0; i < stmt.generic_params.size(); ++i) {
            out << "typename " << stmt.generic_params[i].lexeme;
            if (i < stmt.generic_params.size() - 1) {
                out << ", ";
            }
        }
        out << ">\n";
    }
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
