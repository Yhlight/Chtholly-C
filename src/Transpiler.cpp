#include "Transpiler.h"
#include <utility>
#include <stdexcept>
#include <map>
#include "Token.h"
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"

namespace chtholly {

// A simple visitor to find all struct and enum declarations in the AST.
class DeclarationScanner : public StmtVisitor, public ExprVisitor {
public:
    std::any visitStructStmt(const StructStmt& stmt) override {
        structs[stmt.name.lexeme] = &stmt;
        return nullptr;
    }
    std::any visitEnumStmt(const EnumStmt& stmt) override {
        enums[stmt.name.lexeme] = &stmt;
        return nullptr;
    }

    std::any visitTraitStmt(const TraitStmt& stmt) override { return nullptr; }

    // --- Stmt visitors ---
    std::any visitBlockStmt(const BlockStmt& stmt) override {
        for (const auto& s : stmt.statements) {
            s->accept(*this);
        }
        return nullptr;
    }
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override {
        stmt.expression->accept(*this);
        return nullptr;
    }
    std::any visitFunctionStmt(const FunctionStmt& stmt) override {
        // Don't recurse into function bodies, we only care about top-level declarations
        return nullptr;
    }
    std::any visitIfStmt(const IfStmt& stmt) override {
        stmt.condition->accept(*this);
        stmt.thenBranch->accept(*this);
        if (stmt.elseBranch) stmt.elseBranch->accept(*this);
        return nullptr;
    }
    std::any visitVarStmt(const VarStmt& stmt) override {
        if (stmt.initializer) stmt.initializer->accept(*this);
        return nullptr;
    }
    std::any visitWhileStmt(const WhileStmt& stmt) override {
        stmt.condition->accept(*this);
        stmt.body->accept(*this);
        return nullptr;
    }
    std::any visitForStmt(const ForStmt& stmt) override {
        if (stmt.initializer) stmt.initializer->accept(*this);
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.increment) stmt.increment->accept(*this);
        stmt.body->accept(*this);
        return nullptr;
    }
    std::any visitReturnStmt(const ReturnStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
        return nullptr;
    }
    std::any visitImportStmt(const ImportStmt& stmt) override { return nullptr; }
    std::any visitSwitchStmt(const SwitchStmt& stmt) override {
        stmt.expression->accept(*this);
        for (const auto& c : stmt.cases) {
            c->accept(*this);
        }
        return nullptr;
    }
    std::any visitCaseStmt(const CaseStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
        stmt.body->accept(*this);
        return nullptr;
    }
    std::any visitBreakStmt(const BreakStmt& stmt) override { return nullptr; }
    std::any visitFallthroughStmt(const FallthroughStmt& stmt) override { return nullptr; }

    // --- Expr visitors ---
    std::any visitBinaryExpr(const BinaryExpr& expr) override {
        expr.left->accept(*this);
        expr.right->accept(*this);
        return nullptr;
    }
    std::any visitUnaryExpr(const UnaryExpr& expr) override {
        expr.right->accept(*this);
        return nullptr;
    }
    std::any visitLiteralExpr(const LiteralExpr& expr) override { return nullptr; }
    std::any visitGroupingExpr(const GroupingExpr& expr) override {
        expr.expression->accept(*this);
        return nullptr;
    }
    std::any visitVariableExpr(const VariableExpr& expr) override { return nullptr; }
    std::any visitAssignExpr(const AssignExpr& expr) override {
        expr.value->accept(*this);
        return nullptr;
    }
    std::any visitCallExpr(const CallExpr& expr) override {
        expr.callee->accept(*this);
        for (const auto& arg : expr.arguments) {
            arg->accept(*this);
        }
        return nullptr;
    }
    std::any visitLambdaExpr(const LambdaExpr& expr) override { return nullptr; } // Simplified
    std::any visitGetExpr(const GetExpr& expr) override {
        expr.object->accept(*this);
        return nullptr;
    }
    std::any visitSetExpr(const SetExpr& expr) override {
        expr.object->accept(*this);
        expr.value->accept(*this);
        return nullptr;
    }
    std::any visitSelfExpr(const SelfExpr& expr) override { return nullptr; }
    std::any visitBorrowExpr(const BorrowExpr& expr) override { return nullptr; }
    std::any visitDerefExpr(const DerefExpr& expr) override {
        expr.expression->accept(*this);
        return nullptr;
    }
    std::any visitStructLiteralExpr(const StructLiteralExpr& expr) override {
        for (const auto& [name, value] : expr.fields) {
            value->accept(*this);
        }
        return nullptr;
    }
    std::any visitArrayLiteralExpr(const ArrayLiteralExpr& expr) override {
        for (const auto& element : expr.elements) {
            element->accept(*this);
        }
        return nullptr;
    }
    std::any visitTypeCastExpr(const TypeCastExpr& expr) override {
        expr.expression->accept(*this);
        return nullptr;
    }


    std::map<std::string, const StructStmt*> structs;
    std::map<std::string, const EnumStmt*> enums;
};

Transpiler::Transpiler() : transpiled_files(new std::set<std::string>()), owns_transpiled_files(true) {
    enterScope(); // Global scope
}

Transpiler::Transpiler(std::set<std::string>* transpiled_files) : transpiled_files(transpiled_files), owns_transpiled_files(false) {
    enterScope(); // Global scope
}

Transpiler::~Transpiler() {
    if (owns_transpiled_files) {
        delete transpiled_files;
    }
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
    if (auto array_literal = dynamic_cast<const ArrayLiteralExpr*>(&expr)) {
        vector_used = true;
        if (array_literal->elements.empty()) {
            return TypeInfo{"std::vector<auto>"};
        }
        TypeInfo element_type = get_type(*array_literal->elements[0]);
        return TypeInfo{"std::vector<" + element_type.name + ">"};
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
            if (has_trait(left_type.name, "operator", it->second)) {
                const StructStmt* s = structs[left_type.name];
                for (const auto& method : s->methods) {
                    if (method->name.lexeme == it->second) {
                        return typeExprToTypeInfo(method->return_type.get());
                    }
                }
            }
        }
    }
    if (auto type_cast_expr = dynamic_cast<const TypeCastExpr*>(&expr)) {
        return typeExprToTypeInfo(type_cast_expr->type.get());
    }
    if (auto borrow_expr = dynamic_cast<const BorrowExpr*>(&expr)) {
        TypeInfo type = get_type(*borrow_expr->expression);
        type.is_ref = true;
        type.is_mut_ref = borrow_expr->isMutable;
        return type;
    }
    if (auto call_expr = dynamic_cast<const CallExpr*>(&expr)) {
        if (auto var_expr = dynamic_cast<const VariableExpr*>(call_expr->callee.get())) {
            if (var_expr->name.lexeme == "input") {
                return TypeInfo{"std::string"};
            }
        }
        if (auto get_expr = dynamic_cast<const GetExpr*>(call_expr->callee.get())) {
            TypeInfo object_type = get_type(*get_expr->object);
            if (object_type.name.rfind("std::optional", 0) == 0) {
                if (get_expr->name.lexeme == "unwarp") {
                    // This is simplified. A real implementation would extract the inner type.
                    return TypeInfo{"auto"};
                }
                if (get_expr->name.lexeme == "unwarp_or") {
                    return get_type(*call_expr->arguments[0]);
                }
            }
            if (object_type.name.rfind("Result", 0) == 0) {
                if (get_expr->name.lexeme == "unwarp") {
                    return TypeInfo{"auto"};
                }
                if (get_expr->name.lexeme == "unwarp_err") {
                    return TypeInfo{"auto"};
                }
            }
             if (auto var_expr = dynamic_cast<const VariableExpr*>(get_expr->object.get())) {
                if (var_expr->name.lexeme == "result") {
                    if (get_expr->name.lexeme == "pass") {
                        TypeInfo type_arg = get_type(*call_expr->arguments[0]);
                        return TypeInfo{"Result<" + type_arg.name + ", auto>"};
                    }
                    if (get_expr->name.lexeme == "fail") {
                        TypeInfo type_arg = get_type(*call_expr->arguments[0]);
                        return TypeInfo{"Result<auto, " + type_arg.name + ">"};
                    }
                }
                if (var_expr->name.lexeme == "meta") {
                    return TypeInfo{"bool"};
                }
            if (var_expr->name.lexeme == "reflect") {
                if (get_expr->name.lexeme == "get_field_count" || get_expr->name.lexeme == "get_method_count") {
                    return TypeInfo{"int"};
                }
                if (get_expr->name.lexeme == "get_fields") {
                    return TypeInfo{"std::vector<chtholly_field>"};
                }
                if (get_expr->name.lexeme == "get_methods") {
                    return TypeInfo{"std::vector<chtholly_method>"};
                }
            }
            if (var_expr->name.lexeme == "util") {
                if (get_expr->name.lexeme == "string_cast") {
                    return TypeInfo{"std::string"};
                }
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
        TypeInfo info = typeExprToTypeInfo(borrowType->element_type.get());
        info.is_ref = true;
        info.is_mut_ref = borrowType->isMutable;
        return info;
    }
    if (auto arrayType = dynamic_cast<const ArrayTypeExpr*>(type)) {
        return TypeInfo{ transpileType(*arrayType) };
    }
    if (auto genericType = dynamic_cast<const GenericTypeExpr*>(type)) {
        return TypeInfo{ transpileType(*genericType) };
    }

    // Fallback for more complex types not yet handled.
    return TypeInfo{ "auto" };
}

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    // Pre-scan for struct and enum definitions.
    DeclarationScanner scanner;
    for (const auto& statement : statements) {
        statement->accept(scanner);
    }
    this->structs = scanner.structs;
    this->enums = scanner.enums;

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
    if (vector_used) {
        final_code << "#include <vector>\n";
    }
    if (array_used) {
        final_code << "#include <array>\n";
    }
    if (optional_used) {
        final_code << "#include <optional>\n";
    }
    if (result_used) {
        final_code << "#include \"Result.h\"\n";
    }
    if (reflect_used) {
        final_code << "#include <string>\n";
        final_code << "#include <vector>\n";
        final_code << R"(
struct chtholly_field {
    std::string name;
    std::string type;
};

struct chtholly_parameter {
    std::string name;
    std::string type;
};

struct chtholly_method {
    std::string name;
    std::string return_type;
    std::vector<chtholly_parameter> parameters;
};
)";
    }
    // Add other includes here as needed

    final_code << out.str();
    return final_code.str();
}

std::string Transpiler::getTypeString(const TypeInfo& type) {
    std::string s = type.name;
    if (type.is_ref) {
        s += "&";
    }
    return s;
}

// Helper function to check if a struct implements a specific trait.
bool Transpiler::has_trait(const std::string& struct_name, const std::string& module_name, const std::string& trait_name) {
    if (structs.count(struct_name)) {
        const StructStmt* s = structs[struct_name];
        for (const auto& trait : s->traits) {
            if (auto get_expr = dynamic_cast<const GetExpr*>(trait.get())) {
                if (auto var_expr = dynamic_cast<const VariableExpr*>(get_expr->object.get())) {
                    if (var_expr->name.lexeme == module_name && get_expr->name.lexeme == trait_name) {
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
        {TokenType::AND, "and"}, {TokenType::OR, "or"},
        {TokenType::PLUS_EQUAL, "assign_add"}, {TokenType::MINUS_EQUAL, "assign_sub"}, {TokenType::STAR_EQUAL, "assign_mul"},
        {TokenType::SLASH_EQUAL, "assign_div"}, {TokenType::PERCENT_EQUAL, "assign_mod"}
    };

    auto it = binary_op_traits.find(expr.op.type);
    if (it != binary_op_traits.end()) {
        TypeInfo left_type = get_type(*expr.left);
        if (has_trait(left_type.name, "operator", it->second)) {
            return std::any_cast<std::string>(expr.left->accept(*this)) + "." + it->second + "(" + std::any_cast<std::string>(expr.right->accept(*this)) + ")";
        }
    }

    return std::any_cast<std::string>(expr.left->accept(*this)) + " " + expr.op.lexeme + " " + std::any_cast<std::string>(expr.right->accept(*this));
}

std::any Transpiler::visitUnaryExpr(const UnaryExpr& expr) {
    static const std::map<TokenType, std::string> prefix_op_traits = {
        {TokenType::PLUS_PLUS, "prefix_add"},
        {TokenType::MINUS_MINUS, "prefix_sub"}
    };
    static const std::map<TokenType, std::string> postfix_op_traits = {
        {TokenType::PLUS_PLUS, "postfix_add"},
        {TokenType::MINUS_MINUS, "postfix_sub"}
    };
    static const std::map<TokenType, std::string> other_unary_op_traits = {
        {TokenType::BANG, "not"}
    };


    if (expr.op.type == TokenType::PLUS_PLUS || expr.op.type == TokenType::MINUS_MINUS) {
        const auto& trait_map = expr.is_postfix ? postfix_op_traits : prefix_op_traits;
        auto it = trait_map.find(expr.op.type);
        TypeInfo right_type = get_type(*expr.right);
        if (has_trait(right_type.name, "operator", it->second)) {
            return std::any_cast<std::string>(expr.right->accept(*this)) + "." + it->second + "()";
        }

        // Default to standard C++ operators
        if (expr.is_postfix) {
            return std::any_cast<std::string>(expr.right->accept(*this)) + expr.op.lexeme;
        } else {
            return expr.op.lexeme + std::any_cast<std::string>(expr.right->accept(*this));
        }
    }

    // Handle other unary operators like '!'
    auto it = other_unary_op_traits.find(expr.op.type);
    if (it != other_unary_op_traits.end()) {
        TypeInfo right_type = get_type(*expr.right);
        if (has_trait(right_type.name, "operator", it->second)) {
            return std::any_cast<std::string>(expr.right->accept(*this)) + "." + it->second + "()";
        }
    }

    // Default for all other unary operators (like '-')
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
    if (function_name == "is_array") {
        return std::string(arg_type.name.rfind("std::vector", 0) == 0 || arg_type.name.rfind("std::array", 0) == 0 ? "true" : "false");
    }
    if (function_name == "is_let") {
        return std::string(!arg_type.is_mutable ? "true" : "false");
    }
    if (function_name == "is_mut") {
        return std::string(arg_type.is_mutable ? "true" : "false");
    }
    if (function_name == "is_borrow") {
        return std::string(arg_type.is_ref ? "true" : "false");
    }
    if (function_name == "is_borrow_mut") {
        return std::string(arg_type.is_mut_ref ? "true" : "false");
    }
    if (function_name == "is_move") {
        return std::string(!arg_type.is_ref ? "true" : "false");
    }

    return std::string("/* ERROR: Unknown meta function call */");
}

std::any Transpiler::handleReflectFunction(const CallExpr& expr) {
    auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get());
    std::string function_name = get_expr->name.lexeme;

    if (expr.arguments.empty()) {
        return std::string("/* ERROR: reflect function requires one argument */");
    }
    TypeInfo arg_type = get_type(*expr.arguments[0]);

    if (function_name == "get_field_count") {
        if (structs.count(arg_type.name)) {
            const StructStmt* s = structs[arg_type.name];
            return std::to_string(s->fields.size());
        } else {
            return std::string("0"); // Return 0 if not a struct
        }
    }
    if (function_name == "get_method_count") {
        if (structs.count(arg_type.name)) {
            const StructStmt* s = structs[arg_type.name];
            return std::to_string(s->methods.size());
        } else {
            return std::string("0"); // Return 0 if not a struct
        }
    }
    if (function_name == "get_fields") {
        reflect_used = true;
        if (structs.count(arg_type.name)) {
            const StructStmt* s = structs[arg_type.name];
            std::stringstream ss;
            ss << "std::vector<chtholly_field>{";
            for (size_t i = 0; i < s->fields.size(); ++i) {
                ss << "{\"" << s->fields[i]->name.lexeme << "\", \"" << transpileType(*s->fields[i]->type) << "\"}";
                if (i < s->fields.size() - 1) {
                    ss << ", ";
                }
            }
            ss << "}";
            return ss.str();
        } else {
            return std::string("std::vector<chtholly_field>{}");
        }
    }
    if (function_name == "get_methods") {
        reflect_used = true;
        if (structs.count(arg_type.name)) {
            const StructStmt* s = structs[arg_type.name];
            std::stringstream ss;
            ss << "std::vector<chtholly_method>{";
            for (size_t i = 0; i < s->methods.size(); ++i) {
                ss << "{\"" << s->methods[i]->name.lexeme << "\", \"" << (s->methods[i]->return_type ? transpileType(*s->methods[i]->return_type) : "void") << "\", {";
                for (size_t j = 0; j < s->methods[i]->params.size(); ++j) {
                    ss << "{\"" << s->methods[i]->params[j].lexeme << "\", \"" << transpileType(*s->methods[i]->param_types[j]) << "\"}";
                    if (j < s->methods[i]->params.size() - 1) {
                        ss << ", ";
                    }
                }
                ss << "}}";
                if (i < s->methods.size() - 1) {
                    ss << ", ";
                }
            }
            ss << "}";
            return ss.str();
        } else {
            return std::string("std::vector<chtholly_method>{}");
        }
    }

    return std::string("/* ERROR: Unknown reflect function call */");
}

std::any Transpiler::handleUtilFunction(const CallExpr& expr) {
    auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get());
    std::string function_name = get_expr->name.lexeme;

    if (function_name == "string_cast") {
        if (expr.arguments.empty()) {
            return std::string("/* ERROR: string_cast requires one argument */");
        }
        TypeInfo arg_type = get_type(*expr.arguments[0]);
        if (arg_type.name == "int" || arg_type.name == "double" || arg_type.name == "unsigned int") {
            return "std::to_string(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
        }
        if (has_trait(arg_type.name, "util", "to_string")) {
            return std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ".to_string()";
        }
    }

    return std::string("/* ERROR: Unknown util function call */");
}

std::any Transpiler::visitCallExpr(const CallExpr& expr) {
    if (auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get())) {
        TypeInfo object_type = get_type(*get_expr->object);
        if (object_type.name.rfind("std::optional", 0) == 0) {
            if (get_expr->name.lexeme == "unwarp") {
                return std::any_cast<std::string>(get_expr->object->accept(*this)) + ".value()";
            }
            if (get_expr->name.lexeme == "unwarp_or") {
                return std::any_cast<std::string>(get_expr->object->accept(*this)) + ".value_or(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
            }
        }
        if (object_type.name.rfind("Result", 0) == 0) {
            if (get_expr->name.lexeme == "is_pass") {
                return std::any_cast<std::string>(get_expr->object->accept(*this)) + ".is_pass()";
            }
            if (get_expr->name.lexeme == "is_fail") {
                return std::any_cast<std::string>(get_expr->object->accept(*this)) + ".is_fail()";
            }
        }
        if (auto var_expr = dynamic_cast<const VariableExpr*>(get_expr->object.get())) {
            if (var_expr->name.lexeme == "result") {
                if (get_expr->name.lexeme == "pass") {
                    TypeInfo type = contextual_type.name.empty() ? get_type(expr) : contextual_type;
                    return type.name + "::pass(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
                }
                if (get_expr->name.lexeme == "fail") {
                    TypeInfo type = contextual_type.name.empty() ? get_type(expr) : contextual_type;
                    return type.name + "::fail(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
                }
                if (get_expr->name.lexeme == "is_pass") {
                    return "(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ").is_pass()";
                }
                if (get_expr->name.lexeme == "is_fail") {
                    return "(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ").is_fail()";
                }
            }
            if (var_expr->name.lexeme == "meta") {
                return handleMetaFunction(expr);
            }
            if (var_expr->name.lexeme == "reflect") {
                return handleReflectFunction(expr);
            }
            if (var_expr->name.lexeme == "util") {
                return handleUtilFunction(expr);
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
    if (auto var_expr = dynamic_cast<const VariableExpr*>(expr.object.get())) {
        if (enums.count(var_expr->name.lexeme)) {
            separator = "::";
        }
    } else if (dynamic_cast<const SelfExpr*>(expr.object.get())) {
        separator = "->";
    }
    return std::any_cast<std::string>(expr.object->accept(*this)) + separator + expr.name.lexeme;
}
std::any Transpiler::visitSetExpr(const SetExpr& expr) {
    if (expr.name.type == TokenType::STAR) {
        return "*" + std::any_cast<std::string>(expr.object->accept(*this)) + " = " + std::any_cast<std::string>(expr.value->accept(*this));
    }

    std::string separator = ".";
    if (dynamic_cast<const SelfExpr*>(expr.object.get())) {
        separator = "->";
    }

    return std::any_cast<std::string>(expr.object->accept(*this)) + separator + expr.name.lexeme + " = " + std::any_cast<std::string>(expr.value->accept(*this));
}
std::any Transpiler::visitSelfExpr(const SelfExpr& expr) {
    return std::string("this");
}
std::any Transpiler::visitBorrowExpr(const BorrowExpr& expr) {
    return std::string("&") + std::any_cast<std::string>(expr.expression->accept(*this));
}
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

std::any Transpiler::visitArrayLiteralExpr(const ArrayLiteralExpr& expr) {
    std::stringstream out;
    out << "{";
    for (size_t i = 0; i < expr.elements.size(); ++i) {
        out << std::any_cast<std::string>(expr.elements[i]->accept(*this));
        if (i < expr.elements.size() - 1) {
            out << ", ";
        }
    }
    out << "}";
    return out.str();
}

std::any Transpiler::visitTypeCastExpr(const TypeCastExpr& expr) {
    return "static_cast<" + transpileType(*expr.type) + ">(" + std::any_cast<std::string>(expr.expression->accept(*this)) + ")";
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
    type.is_mutable = stmt.isMutable;

    define(stmt.name.lexeme, type);

    out << (type.is_mutable ? "" : "const ") << getTypeString(type) << " " << stmt.name.lexeme;
    if (stmt.initializer) {
        TypeInfo old_context = contextual_type;
        contextual_type = type;
        out << " = " << std::any_cast<std::string>(stmt.initializer->accept(*this));
        contextual_type = old_context;
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

std::any Transpiler::visitForStmt(const ForStmt& stmt) {
    enterScope();
    out << "for (";
    if (stmt.initializer) {
        if (auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmt.initializer.get())) {
            out << std::any_cast<std::string>(exprStmt->expression->accept(*this));
        } else if (auto* varStmt = dynamic_cast<VarStmt*>(stmt.initializer.get())) {
            // Slightly simplified VarStmt handling for for-loops
             TypeInfo type = typeExprToTypeInfo(varStmt->type.get());
            if (varStmt->initializer && type.name == "auto") {
                type = get_type(*varStmt->initializer);
            }

            define(varStmt->name.lexeme, type);

            out << (type.is_mutable ? "" : "const ") << getTypeString(type) << " " << varStmt->name.lexeme;
            if (varStmt->initializer) {
                out << " = " << std::any_cast<std::string>(varStmt->initializer->accept(*this));
            }
        }
    }
    out << "; ";
    if (stmt.condition) {
        out << std::any_cast<std::string>(stmt.condition->accept(*this));
    }
    out << "; ";
    if (stmt.increment) {
        out << std::any_cast<std::string>(stmt.increment->accept(*this));
    }
    out << ") ";
    stmt.body->accept(*this);
    exitScope();
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
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::any Transpiler::visitImportStmt(const ImportStmt& stmt) {
    if (std::holds_alternative<Token>(stmt.path)) {
        std::string module_name = std::get<Token>(stmt.path).lexeme;
        imported_modules.insert(module_name);
    } else if (std::holds_alternative<std::string>(stmt.path)) {
        std::string file_path = std::get<std::string>(stmt.path);

        if (transpiled_files->find(file_path) == transpiled_files->end()) {
            transpiled_files->insert(file_path);
            std::string source = readFile(file_path);

            Lexer lexer(source);
            std::vector<Token> tokens = lexer.scanTokens();
            Parser parser(tokens);
            std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

            Transpiler import_transpiler(transpiled_files);
            out << import_transpiler.transpile(statements);
        }
    }
    return nullptr;
}
std::any Transpiler::visitSwitchStmt(const SwitchStmt& stmt) {
    is_in_switch = true;
    out << "switch (" << std::any_cast<std::string>(stmt.expression->accept(*this)) << ") {\n";
    for (const auto& case_stmt : stmt.cases) {
        case_stmt->accept(*this);
    }
    out << "}\n";
    is_in_switch = false;
    return nullptr;
}
std::any Transpiler::visitCaseStmt(const CaseStmt& stmt) {
    if (stmt.value) {
        out << "case " << std::any_cast<std::string>(stmt.value->accept(*this)) << ":\n";
    } else {
        out << "default:\n";
    }
    stmt.body->accept(*this);

    // Add implicit break if there's no fallthrough, break, or return.
    bool has_terminator = false;
    if (const auto* block = dynamic_cast<const BlockStmt*>(stmt.body.get())) {
        if (!block->statements.empty()) {
            const auto& last_stmt = block->statements.back();
            if (dynamic_cast<const FallthroughStmt*>(last_stmt.get()) ||
                dynamic_cast<const BreakStmt*>(last_stmt.get()) ||
                dynamic_cast<const ReturnStmt*>(last_stmt.get())) {
                has_terminator = true;
            }
        }
    }

    if (!has_terminator) {
        out << "break;\n";
    }
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
        if (baseType->type.lexeme == "option") {
            optional_used = true;
            return "std::optional";
        }
        if (baseType->type.lexeme == "result") {
            result_used = true;
            return "Result";
        }
        if (baseType->type.lexeme == "int") return "int";
        if (baseType->type.lexeme == "uint") return "unsigned int";
        if (baseType->type.lexeme == "string") return "std::string";
        if (baseType->type.lexeme == "bool") return "bool";
        if (baseType->type.lexeme == "void") return "void";
        return baseType->type.lexeme;
    }
    if (auto genericType = dynamic_cast<const GenericTypeExpr*>(&type)) {
        std::string base_name = genericType->base_type.lexeme;
        if (base_name == "option") {
            optional_used = true;
            base_name = "std::optional";
        }
        if (base_name == "result") {
            result_used = true;
            base_name = "Result";
        }
        std::string s = base_name + "<";
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
    if (auto arrayType = dynamic_cast<const ArrayTypeExpr*>(&type)) {
        if (arrayType->size) {
            array_used = true;
            return "std::array<" + transpileType(*arrayType->element_type) + ", " + std::any_cast<std::string>(arrayType->size->accept(*this)) + ">";
        } else {
            vector_used = true;
            return "std::vector<" + transpileType(*arrayType->element_type) + ">";
        }
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
    bool first_param = true;
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        if (stmt.params[i].lexeme == "self") continue;
        if (!first_param) out << ", ";
        first_param = false;

        TypeInfo paramType = typeExprToTypeInfo(stmt.param_types[i].get());
        define(stmt.params[i].lexeme, paramType);
        out << getTypeString(paramType) << " " << stmt.params[i].lexeme;
    }
    out << ") ";

    if (stmt.body) {
        visitBlockStmt(*stmt.body, false);
    } else {
        out << ";\n";
    }

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
        is_in_method = true;
        out << (method->return_type ? transpileType(*method->return_type) : "void") << " " << method->name.lexeme << "(";

        bool first_param = true;
        for (size_t i = 0; i < method->params.size(); ++i) {
            if (method->params[i].lexeme == "self") continue;
            if (!first_param) out << ", ";
            first_param = false;
            out << transpileType(*method->param_types[i]) << " " << method->params[i].lexeme;
        }
        out << ") ";
        if (method->body) {
            method->body->accept(*this);
        } else {
            out << ";\n";
        }
        is_in_method = false;
    }
    out << "};\n";
    return nullptr;
}

std::any Transpiler::visitEnumStmt(const EnumStmt& stmt) {
    out << "enum class " << stmt.name.lexeme << " {\n";
    for (size_t i = 0; i < stmt.members.size(); ++i) {
        out << "    " << stmt.members[i].lexeme;
        if (i < stmt.members.size() - 1) {
            out << ",";
        }
        out << "\n";
    }
    out << "};\n";
    return nullptr;
}

std::any Transpiler::visitTraitStmt(const TraitStmt& stmt) {
    out << "struct " << stmt.name.lexeme << " {\n";
    out << "    virtual ~" << stmt.name.lexeme << "() = default;\n";
    for (const auto& method : stmt.methods) {
        out << "    virtual " << (method->return_type ? transpileType(*method->return_type) : "void")
            << " " << method->name.lexeme << "(";
        bool first_param = true;
        for (size_t i = 0; i < method->params.size(); ++i) {
            if (method->params[i].lexeme == "self") continue;
            if (!first_param) out << ", ";
            first_param = false;
            out << transpileType(*method->param_types[i]) << " " << method->params[i].lexeme;
        }
        out << ") = 0;\n";
    }
    out << "};\n";
    return nullptr;
}

} // namespace chtholly
