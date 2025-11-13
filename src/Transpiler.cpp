#include "Transpiler.h"
#include <utility>
#include <stdexcept>
#include <map>
#include "Token.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include "Lexer.h"
#include "Parser.h"
#include "result.h"

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
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::map<std::string, std::unique_ptr<Expr>>>) {
                for (const auto& [name, value] : arg) {
                    value->accept(*this);
                }
            } else if constexpr (std::is_same_v<T, std::vector<std::unique_ptr<Expr>>>) {
                for (const auto& value : arg) {
                    value->accept(*this);
                }
            }
        }, expr.initializers);
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

Transpiler::Transpiler(bool is_main_file) : is_main_file(is_main_file) {
    transpiled_files = new std::set<std::string>();
    owns_transpiled_files = true;
}

Transpiler::Transpiler(std::set<std::string>* transpiled_files, bool is_main_file)
    : transpiled_files(transpiled_files), owns_transpiled_files(false), is_main_file(is_main_file) {
}

Transpiler::~Transpiler() {
    if (owns_transpiled_files) {
        delete transpiled_files;
    }
}

// Helper to convert the new Type system to the old TypeInfo.
// This is a temporary measure during refactoring.
TypeInfo typeToTypeInfo(const std::shared_ptr<Type>& type) {
    if (!type) return TypeInfo{"auto"};

    if (auto basic = std::dynamic_pointer_cast<BasicType>(type)) {
        if (basic->get_name() == "string") return TypeInfo{"std::string"};
        return TypeInfo{basic->get_name()};
    }
    if (auto struct_type = std::dynamic_pointer_cast<StructType>(type)) {
        if (struct_type->get_name() == "option") {
            // This is a simplified handling for os::env returning option<string>
            return TypeInfo{"std::optional<std::string>"};
        }
        if (struct_type->get_name() == "option<int>") {
            return TypeInfo{"std::optional<int>"};
        }
        return TypeInfo{struct_type->get_name()};
    }
    if (auto array_type = std::dynamic_pointer_cast<ArrayType>(type)) {
        TypeInfo element_type = typeToTypeInfo(array_type->get_element_type());
        if (element_type.name == "void") {
            return TypeInfo{"std::vector<int>"}; // Default empty array to vector<int>
        }
        return TypeInfo{"std::vector<" + element_type.name + ">"};
    }
    if (auto borrow_type = std::dynamic_pointer_cast<BorrowType>(type)) {
        TypeInfo element_type = typeToTypeInfo(borrow_type->get_element_type());
        element_type.is_ref = true;
        element_type.is_mut_ref = borrow_type->is_mutable();
        return element_type;
    }
    // Add more conversions as needed (FunctionType, etc.)

    return TypeInfo{"auto"};
}

TypeInfo Transpiler::get_type(const Expr& expr) {
    // The ONLY source of truth for types is the `type` property on the expression,
    // which is populated by the Resolver.
    if (expr.type) {
        return typeToTypeInfo(expr.type);
    }

    // Fallback for literals, which don't need the resolver.
    if (auto literal_expr = dynamic_cast<const LiteralExpr*>(&expr)) {
        if (std::holds_alternative<long long>(literal_expr->value)) return TypeInfo{"int"};
        if (std::holds_alternative<double>(literal_expr->value)) return TypeInfo{"double"};
        if (std::holds_alternative<std::string>(literal_expr->value)) return TypeInfo{"std::string"};
        if (std::holds_alternative<bool>(literal_expr->value)) return TypeInfo{"bool"};
        if (std::holds_alternative<char>(literal_expr->value)) return TypeInfo{"char"};
    }

    // If the resolver didn't assign a type, we can't know it.
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
    if (auto funcType = dynamic_cast<const FunctionTypeExpr*>(type)) {
        return TypeInfo{ transpileType(*funcType) };
    }

    // Fallback for more complex types not yet handled.
    return TypeInfo{ "auto" };
}

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    // Reset state for clean transpilation between runs (especially for tests)
    out.str("");
    out.clear();
    structs.clear();
    enums.clear();
    imported_modules.clear();
    if (owns_transpiled_files) {
        transpiled_files->clear();
    }
    input_used = false;
    vector_used = false;
    array_used = false;
    optional_used = false;
    result_used = false;
    result_static_check_used = false;
    function_used = false;
    string_helpers_used = false;
    string_used = false;
    type_traits_used = false;
    reflect_used = false;
    is_in_method = false;
    current_struct = nullptr;
    current_function_return_type = nullptr;
    contextual_type = TypeInfo{};

    // First pass: handle all import statements.
    for (const auto& statement : statements) {
        if (dynamic_cast<const ImportStmt*>(statement.get())) {
            statement->accept(*this);
        }
    }

    // Second pass: scan for declarations in the current file + imported ones.
    DeclarationScanner scanner;
    for (const auto& statement : statements) {
        statement->accept(scanner);
    }
    this->structs.insert(scanner.structs.begin(), scanner.structs.end());
    this->enums.insert(scanner.enums.begin(), scanner.enums.end());


    // Third pass: transpile the rest of the statements.
    for (const auto& statement : statements) {
        if (!dynamic_cast<const ImportStmt*>(statement.get())) {
            statement->accept(*this);
        }
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
        final_code << "#include <filesystem>\n";
        final_code << "#include <vector>\n";
        final_code << "#include <string>\n";
        final_code << R"(
void chtholly_fs_write(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    file << content;
}

std::string chtholly_fs_read(const std::string& path) {
    std::ifstream file(path);
    if (!file) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool chtholly_fs_exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool chtholly_fs_is_file(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

bool chtholly_fs_is_dir(const std::string& path) {
    return std::filesystem::is_directory(path);
}

std::vector<std::string> chtholly_fs_list_dir(const std::string& path) {
    std::vector<std::string> entries;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        entries.push_back(entry.path().u8string());
    }
    return entries;
}

void chtholly_fs_remove(const std::string& path) {
    std::filesystem::remove_all(path);
}
)";
    }
    if (imported_modules.count("math")) {
        final_code << "#include <cmath>\n";
    }
    if (imported_modules.count("random")) {
        final_code << "#include <random>\n";
    }
    if (imported_modules.count("cstdlib")) {
        final_code << "#include <cstdlib>\n";
    }
    if (imported_modules.count("algorithm")) {
        final_code << "#include <algorithm>\n";
    }
    if (imported_modules.count("chrono")) {
        final_code << "#include <chrono>\n";
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
        final_code << "#include \"result.h\"\n";
    }
    if (result_static_check_used) {
        final_code << R"(
#include <type_traits>

template<class>
struct is_result : std::false_type {};

template<class T, class E>
struct is_result<result<T, E>> : std::true_type {};

template<typename T>
constexpr bool chtholly_is_pass(const T& value) {
    if constexpr (is_result<T>::value) {
        return value.is_pass();
    } else {
        return false;
    }
}

template<typename T>
constexpr bool chtholly_is_fail(const T& value) {
    if constexpr (is_result<T>::value) {
        return value.is_fail();
    } else {
        return false;
    }
}
)";
    }
    if (function_used) {
        final_code << "#include <functional>\n";
    }
    if (string_helpers_used) {
        final_code << "#include <string>\n";
        final_code << "#include <vector>\n";
        final_code << "#include <sstream>\n";
        final_code << "#include <algorithm>\n";
        final_code << "#include <cctype>\n";
        final_code << R"(
std::vector<std::string> chtholly_string_split(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = s.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }
    tokens.push_back(s.substr(start));
    return tokens;
}

std::string chtholly_string_join(const std::vector<std::string>& elements, const std::string& separator) {
    std::stringstream ss;
    for (size_t i = 0; i < elements.size(); ++i) {
        ss << elements[i];
        if (i < elements.size() - 1) {
            ss << separator;
        }
    }
    return ss.str();
}

std::string chtholly_string_to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::toupper(c); });
    return s;
}

std::string chtholly_string_to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

std::string chtholly_string_trim(const std::string& s) {
    if (s.empty()) {
        return "";
    }
    auto start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    auto end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

std::string chtholly_string_replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}
)";
    } else if (string_used) {
        final_code << "#include <string>\n";
    }

    if (type_traits_used) {
        final_code << "#include <type_traits>\n";
    }
    if (reflect_used) {
        final_code << "#include <string>\n";
        final_code << "#include <vector>\n";
        final_code << R"(
struct Field {
    std::string name;
    std::string type;
};

struct Parameter {
    std::string name;
    std::string type;
};

struct Method {
    std::string name;
    std::string return_type;
    std::vector<Parameter> parameters;
};

struct Trait {
    std::string name;
};
)";
    }
    // Add other includes here as needed

    final_code << out.str();

    if (is_main_file) {
        final_code << R"(
#include <vector>
#include <string>

// Forward declare chtholly_main
int chtholly_main(std::vector<std::string> args);

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return chtholly_main(args);
}
)";
    }

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
    if (expr.op.type == TokenType::STAR_STAR) {
        TypeInfo left_type = get_type(*expr.left);
        if (has_trait(left_type.name, "operator", "binary")) {
            string_used = true;
            return std::any_cast<std::string>(expr.left->accept(*this)) + ".binary(\"**\", " + std::any_cast<std::string>(expr.right->accept(*this)) + ")";
        }
        // C++ has no native ** operator, so if the trait is not implemented, it's an error.
        return "/* ERROR: operator '**' is not defined for type '" + left_type.name + "' */";
    }

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
        std::stringstream ss;
        ss << std::fixed << std::get<double>(expr.value);
        std::string s = ss.str();
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') {
            s.push_back('0');
        }
        return s;
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
    if (is_in_method && current_struct) {
        for (const auto& field : current_struct->fields) {
            if (field->name.lexeme == expr.name.lexeme) {
                return "/* ERROR: Member '" + expr.name.lexeme + "' cannot be accessed without 'self.'. */";
            }
        }
    }
    return expr.name.lexeme;
}

std::any Transpiler::visitAssignExpr(const AssignExpr& expr) {
    if (is_in_method && current_struct) {
        for (const auto& field : current_struct->fields) {
            if (field->name.lexeme == expr.name.lexeme) {
                return "/* ERROR: Member '" + expr.name.lexeme + "' cannot be accessed without 'self.'. */";
            }
        }
    }
    return expr.name.lexeme + " = " + std::any_cast<std::string>(expr.value->accept(*this));
}

// Placeholder for now
std::any Transpiler::handleMetaFunction(const CallExpr& expr) {
    auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get());
    std::string function_name = get_expr->name.lexeme;
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
    if (function_name == "type_name") {
        std::string typeName = arg_type.name;
        if (typeName == "std::string") {
            typeName = "string";
        }
        return "\"" + typeName + "\"";
    }

    return std::string("/* ERROR: Unknown meta function call */");
}

std::any Transpiler::handleReflectFunction(const CallExpr& expr) {
    auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get());
    std::string function_name = get_expr->name.lexeme;
    TypeInfo arg_type = get_type(*expr.arguments[0]);
    const StructStmt* s = structs[arg_type.name];

    if (function_name == "get_field_count") {
        return std::to_string(s->fields.size());
    }
    if (function_name == "get_method_count") {
        return std::to_string(s->methods.size());
    }
    if (function_name == "get_fields") {
        reflect_used = true;
        std::stringstream ss;
        ss << "std::vector<Field>{";
        for (size_t i = 0; i < s->fields.size(); ++i) {
            ss << "{\"" << s->fields[i]->name.lexeme << "\", \"" << transpileType(*s->fields[i]->type) << "\"}";
            if (i < s->fields.size() - 1) {
                ss << ", ";
            }
        }
        ss << "}";
        return ss.str();
    }
    if (function_name == "get_field") {
        reflect_used = true;
        auto field_name_expr = dynamic_cast<const LiteralExpr*>(expr.arguments[1].get());
        std::string field_name = std::get<std::string>(field_name_expr->value);
        for (const auto& field : s->fields) {
            if (field->name.lexeme == field_name) {
                return "{\"" + field->name.lexeme + "\", \"" + transpileType(*field->type) + "\"}";
            }
        }
        return std::string("/* ERROR: field not found */");
    }
    if (function_name == "get_methods") {
        reflect_used = true;
        std::stringstream ss;
        ss << "std::vector<Method>{";
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
    }
    if (function_name == "get_method") {
        reflect_used = true;
        auto method_name_expr = dynamic_cast<const LiteralExpr*>(expr.arguments[1].get());
        std::string method_name = std::get<std::string>(method_name_expr->value);
        for (const auto& method : s->methods) {
            if (method->name.lexeme == method_name) {
                std::stringstream ss;
                ss << "{\"" << method->name.lexeme << "\", \"" << (method->return_type ? transpileType(*method->return_type) : "void") << "\", {";
                for (size_t j = 0; j < method->params.size(); ++j) {
                    ss << "{\"" << method->params[j].lexeme << "\", \"" << transpileType(*method->param_types[j]) << "\"}";
                    if (j < method->params.size() - 1) {
                        ss << ", ";
                    }
                }
                ss << "}}";
                return ss.str();
            }
        }
        return std::string("/* ERROR: method not found */");
    }
    if (function_name == "get_trait_count") {
        return std::to_string(s->traits.size());
    }
    if (function_name == "get_traits") {
        reflect_used = true;
        std::stringstream ss;
        ss << "std::vector<Trait>{";
        for (size_t i = 0; i < s->traits.size(); ++i) {
            if (auto var_expr = dynamic_cast<const VariableExpr*>(s->traits[i].get())) {
                ss << "{\"" << var_expr->name.lexeme << "\"}";
            } else if (auto get_expr = dynamic_cast<const GetExpr*>(s->traits[i].get())) {
                ss << "{\"" << std::any_cast<std::string>(get_expr->object->accept(*this)) << "::" << get_expr->name.lexeme << "\"}";
            }
            if (i < s->traits.size() - 1) {
                ss << ", ";
            }
        }
        ss << "}";
        return ss.str();
    }
    if (function_name == "get_trait") {
        reflect_used = true;
        auto trait_name_expr = dynamic_cast<const LiteralExpr*>(expr.arguments[1].get());
        std::string trait_name = std::get<std::string>(trait_name_expr->value);
        for (const auto& trait : s->traits) {
            std::string current_trait_name;
            if (auto var_expr = dynamic_cast<const VariableExpr*>(trait.get())) {
                current_trait_name = var_expr->name.lexeme;
            } else if (auto get_expr = dynamic_cast<const GetExpr*>(trait.get())) {
                current_trait_name = std::any_cast<std::string>(get_expr->object->accept(*this)) + "::" + get_expr->name.lexeme;
            }
            if (current_trait_name == trait_name) {
                return "{\"" + current_trait_name + "\"}";
            }
        }
        return std::string("/* ERROR: trait not found */");
    }

    return std::string("/* ERROR: Unknown reflect function call */");
}

std::any Transpiler::handleUtilFunction(const CallExpr& expr) {
    auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get());
    std::string function_name = get_expr->name.lexeme;

    if (function_name == "string_cast") {
        TypeInfo arg_type = get_type(*expr.arguments[0]);
        if (arg_type.name == "int" || arg_type.name == "double" || arg_type.name == "unsigned int") {
            return "std::to_string(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
        }
        if (has_trait(arg_type.name, "util", "to_string")) {
            return std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ".to_string()";
        }
    }
    if (function_name == "panic") {
        imported_modules.insert("iostream");
        imported_modules.insert("cstdlib");
        return "std::cerr << \"Panic: \" << " + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + " << std::endl; std::exit(1);";
    }

    return std::string("/* ERROR: Unknown util function call */");
}

std::any Transpiler::handleMathFunction(const CallExpr& expr) {
    auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get());
    std::string function_name = get_expr->name.lexeme;

    // Functions with one argument
    if (function_name == "sqrt" || function_name == "sin" || function_name == "cos" ||
        function_name == "tan" || function_name == "log" || function_name == "log10" ||
        function_name == "abs" || function_name == "ceil" || function_name == "floor" ||
        function_name == "round") {
        return "std::" + function_name + "(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
    }

    // Functions with two arguments
    if (function_name == "pow") {
        return "std::pow(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ", " +
               std::any_cast<std::string>(expr.arguments[1]->accept(*this)) + ")";
    }

    return "/* ERROR: Unknown math function call */";
}

std::any Transpiler::handleStringMethodCall(const CallExpr& expr, const GetExpr& get_expr) {
    std::string object_str = std::any_cast<std::string>(get_expr.object->accept(*this));
    std::string function_name = get_expr.name.lexeme;

    // Check if the object expression results in a string literal.
    // This is needed to wrap literals in std::string() for member access.
    const Expr* base_expr = get_expr.object.get();
    if (auto group = dynamic_cast<const GroupingExpr*>(base_expr)) {
        base_expr = group->expression.get();
    }
    bool is_string_literal = false;
    if (auto literal = dynamic_cast<const LiteralExpr*>(base_expr)) {
        if (std::holds_alternative<std::string>(literal->value)) {
            is_string_literal = true;
        }
    }

    // For direct member calls, wrap literal; for helpers, don't.
    std::string cpp_object = object_str;
    if (is_string_literal) {
        cpp_object = "std::string(" + object_str + ")";
        string_used = true;
    }

    if (function_name == "length") {
        return cpp_object + ".length()";
    }
    if (function_name == "substr") {
        return cpp_object + ".substr(" +
            std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ", " +
            std::any_cast<std::string>(expr.arguments[1]->accept(*this)) + ")";
    }
    if (function_name == "find") {
        optional_used = true;
        string_used = true; // For std::string::npos
        std::string sub = std::any_cast<std::string>(expr.arguments[0]->accept(*this));
        return "(" + cpp_object + ".find(" + sub + ") == std::string::npos) ? std::nullopt : std::optional<int>(" + cpp_object + ".find(" + sub + "))";
    }
    if (function_name == "split") {
        string_helpers_used = true;
        return "chtholly_string_split(" + object_str + ", " + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
    }
    if (function_name == "to_upper") {
        string_helpers_used = true;
        return "chtholly_string_to_upper(" + object_str + ")";
    }
    if (function_name == "to_lower") {
        string_helpers_used = true;
        return "chtholly_string_to_lower(" + object_str + ")";
    }
    if (function_name == "trim") {
        string_helpers_used = true;
        return "chtholly_string_trim(" + object_str + ")";
    }
    if (function_name == "starts_with") {
        std::string prefix = std::any_cast<std::string>(expr.arguments[0]->accept(*this));
        return "([&]() { std::string s_val = " + cpp_object + "; std::string p_val = " + prefix + "; return s_val.find(p_val, 0) == 0; }())";
    }
    if (function_name == "ends_with") {
        std::string suffix = std::any_cast<std::string>(expr.arguments[0]->accept(*this));
        return "([&]() { std::string s_val = " + cpp_object + "; std::string suf_val = " + suffix + "; return s_val.size() >= suf_val.size() && s_val.compare(s_val.size() - suf_val.size(), suf_val.size(), suf_val) == 0; }())";
    }
    if (function_name == "is_empty") {
        return cpp_object + ".empty()";
    }
    if (function_name == "replace") {
        string_helpers_used = true;
        std::string from = std::any_cast<std::string>(expr.arguments[0]->accept(*this));
        std::string to = std::any_cast<std::string>(expr.arguments[1]->accept(*this));
        return "chtholly_string_replace_all(" + object_str + ", " + from + ", " + to + ")";
    }
    if (function_name == "contains") {
        string_used = true;
        std::string sub = std::any_cast<std::string>(expr.arguments[0]->accept(*this));
        return "(" + cpp_object + ".find(" + sub + ") != std::string::npos)";
    }

    return "/* ERROR: Unknown string method call */";
}

std::any Transpiler::handleArrayMethodCall(const CallExpr& expr, const GetExpr& get_expr) {
    std::string object_str = std::any_cast<std::string>(get_expr.object->accept(*this));
    std::string function_name = get_expr.name.lexeme;

    if (function_name == "length") {
        return object_str + ".size()";
    }
    if (function_name == "push") {
        return object_str + ".push_back(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
    }
    if (function_name == "pop") {
        return "[&]() { auto val = " + object_str + ".back(); " + object_str + ".pop_back(); return val; }()";
    }
    if (function_name == "contains") {
        imported_modules.insert("algorithm");
        std::string val = std::any_cast<std::string>(expr.arguments[0]->accept(*this));
        return "(std::find(" + object_str + ".begin(), " + object_str + ".end(), " + val + ") != " + object_str + ".end())";
    }
    if (function_name == "reverse") {
        imported_modules.insert("algorithm");
        return "std::reverse(" + object_str + ".begin(), " + object_str + ".end())";
    }
    if (function_name == "is_empty") {
        return object_str + ".empty()";
    }
    if (function_name == "clear") {
        return object_str + ".clear()";
    }
    if (function_name == "sort") {
        imported_modules.insert("algorithm");
        return "std::sort(" + object_str + ".begin(), " + object_str + ".end())";
    }
    if (function_name == "join") {
        string_helpers_used = true;
        return "chtholly_string_join(" + object_str + ", " + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
    }

    return "/* ERROR: Unknown array method call */";
}

std::any Transpiler::handleOSFunction(const CallExpr& expr) {
    auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get());
    std::string function_name = get_expr->name.lexeme;

    if (function_name == "exit") {
        imported_modules.insert("cstdlib");
        return "std::exit(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
    }
    if (function_name == "env") {
        imported_modules.insert("cstdlib");
        optional_used = true;
        std::string var_name = std::any_cast<std::string>(expr.arguments[0]->accept(*this));
        return "(std::getenv(" + var_name + ") ? std::optional<std::string>(std::getenv(" + var_name + ")) : std::nullopt)";
    }

    return "/* ERROR: Unknown os function call */";
}

std::any Transpiler::handleTimeFunction(const CallExpr& expr) {
    auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get());
    std::string function_name = get_expr->name.lexeme;

    if (function_name == "now") {
        imported_modules.insert("chrono");
        return std::string("std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()");
    }

    return std::string("/* ERROR: Unknown time function call */");
}

std::any Transpiler::handleRandomFunction(const CallExpr& expr) {
    auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get());
    std::string function_name = get_expr->name.lexeme;

    if (function_name == "rand") {
        return std::string("([&]() { std::random_device rd; std::mt19937 gen(rd()); std::uniform_real_distribution<> distrib(0.0, 1.0); return distrib(gen); }())");
    }
    if (function_name == "randint") {
        std::string min = std::any_cast<std::string>(expr.arguments[0]->accept(*this));
        std::string max = std::any_cast<std::string>(expr.arguments[1]->accept(*this));
        return "([&]() { std::random_device rd; std::mt19937 gen(rd()); std::uniform_int_distribution<> distrib(" + min + ", " + max + "); return distrib(gen); }())";
    }

    return "/* ERROR: Unknown random function call */";
}


std::any Transpiler::visitCallExpr(const CallExpr& expr) {
    if (is_in_method && current_struct) {
        if (auto var_expr = dynamic_cast<const VariableExpr*>(expr.callee.get())) {
            for (const auto& method : current_struct->methods) {
                if (method->name.lexeme == var_expr->name.lexeme) {
                    return "/* ERROR: Method '" + var_expr->name.lexeme + "' cannot be called without 'self.'. */";
                }
            }
        }
    }
    if (auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get())) {
        TypeInfo object_type = get_type(*get_expr->object);
        if (object_type.name.rfind("std::optional", 0) == 0) {
            if (get_expr->name.lexeme == "unwrap") {
                return std::any_cast<std::string>(get_expr->object->accept(*this)) + ".value()";
            }
            if (get_expr->name.lexeme == "unwrap_or") {
                return std::any_cast<std::string>(get_expr->object->accept(*this)) + ".value_or(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
            }
        }
        if (object_type.name.rfind("result", 0) == 0) {
            if (get_expr->name.lexeme == "is_pass") {
                return std::any_cast<std::string>(get_expr->object->accept(*this)) + ".is_pass()";
            }
            if (get_expr->name.lexeme == "is_fail") {
                return std::any_cast<std::string>(get_expr->object->accept(*this)) + ".is_fail()";
            }
        }
        if (object_type.name == "std::string") {
            return handleStringMethodCall(expr, *get_expr);
        }
        if (object_type.name.rfind("std::vector", 0) == 0 || object_type.name.rfind("std::array", 0) == 0) {
            return handleArrayMethodCall(expr, *get_expr);
        }
        if (auto var_expr = dynamic_cast<const VariableExpr*>(get_expr->object.get())) {
            if (var_expr->name.lexeme == "result") {
                if (get_expr->name.lexeme == "pass" || get_expr->name.lexeme == "fail") {
                    std::string type_name = "auto";
                    if (!contextual_type.name.empty()) {
                        type_name = contextual_type.name;
                    } else if (current_function_return_type) {
                        type_name = transpileType(*current_function_return_type);
                    } else {
                        type_name = get_type(expr).name;
                    }

                    if (expr.arguments.empty()) {
                        return type_name + "::" + get_expr->name.lexeme + "({})";
                    }
                    return type_name + "::" + get_expr->name.lexeme + "(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
                }
                if (get_expr->name.lexeme == "is_pass") {
                    result_static_check_used = true;
                    return "chtholly_is_pass(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
                }
                if (get_expr->name.lexeme == "is_fail") {
                    result_static_check_used = true;
                    return "chtholly_is_fail(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
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
            if (var_expr->name.lexeme == "math") {
                return handleMathFunction(expr);
            }
            if (var_expr->name.lexeme == "os") {
                return handleOSFunction(expr);
            }
            if (var_expr->name.lexeme == "time") {
                return handleTimeFunction(expr);
            }
            if (var_expr->name.lexeme == "random") {
                return handleRandomFunction(expr);
            }
        }
    }

    if (auto var_expr = dynamic_cast<const VariableExpr*>(expr.callee.get())) {
        if (var_expr->name.lexeme == "option") {
            optional_used = true;
            if (expr.arguments.size() != 1) {
                return std::string("/* ERROR: option() constructor expects exactly one argument */");
            }
            TypeInfo arg_type = get_type(*expr.arguments[0]);
            return "std::optional<" + arg_type.name + ">(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
        }
        if (var_expr->name.lexeme == "print") {
            if (imported_modules.find("iostream") == imported_modules.end()) {
                return std::string("/* ERROR: 'print' function called without importing 'iostream' */");
            }
            std::stringstream out;
            out << "std::cout << ";
            for (size_t i = 0; i < expr.arguments.size(); ++i) {
                if (expr.arguments[i]->type && expr.arguments[i]->type->equals(BasicType("bool"))) {
                    out << "(" << std::any_cast<std::string>(expr.arguments[i]->accept(*this)) << " ? \"true\" : \"false\")";
                } else {
                    out << std::any_cast<std::string>(expr.arguments[i]->accept(*this));
                }
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
        if (var_expr->name.lexeme == "fs_read" || var_expr->name.lexeme == "fs_exists" ||
            var_expr->name.lexeme == "fs_is_file" || var_expr->name.lexeme == "fs_is_dir" ||
            var_expr->name.lexeme == "fs_list_dir" || var_expr->name.lexeme == "fs_remove" ||
            var_expr->name.lexeme == "fs_write") {
            if (imported_modules.find("filesystem") == imported_modules.end()) {
                return "/* ERROR: Filesystem function '" + var_expr->name.lexeme + "' called without importing 'filesystem' */";
            }

            if (var_expr->name.lexeme == "fs_write") {
                if (expr.arguments.size() != 2) {
                    return "/* ERROR: 'fs_write' requires two arguments */";
                }
                return "chtholly_fs_write(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ", " +
                       std::any_cast<std::string>(expr.arguments[1]->accept(*this)) + ")";
            } else {
                if (expr.arguments.size() != 1) {
                    return "/* ERROR: Filesystem function '" + var_expr->name.lexeme + "' expects one argument */";
                }
                return "chtholly_" + var_expr->name.lexeme + "(" + std::any_cast<std::string>(expr.arguments[0]->accept(*this)) + ")";
            }
        }
    }

    std::stringstream out;
    out << std::any_cast<std::string>(expr.callee->accept(*this)) << "(";
    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        if (auto borrow_expr = dynamic_cast<const BorrowExpr*>(expr.arguments[i].get())) {
            out << std::any_cast<std::string>(borrow_expr->expression->accept(*this));
        } else {
            out << std::any_cast<std::string>(expr.arguments[i]->accept(*this));
        }
        if (i < expr.arguments.size() - 1) {
            out << ", ";
        }
    }
    out << ")";
    return out.str();
}
std::any Transpiler::visitLambdaExpr(const LambdaExpr& expr) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < expr.captures.size(); ++i) {
        ss << expr.captures[i].lexeme;
        if (i < expr.captures.size() - 1) {
            ss << ", ";
        }
    }
    ss << "](";

    for (size_t i = 0; i < expr.params.size(); ++i) {
        TypeInfo paramType = typeExprToTypeInfo(expr.param_types[i].get());
        ss << getTypeString(paramType) << " " << expr.params[i].lexeme;
        if (i < expr.params.size() - 1) {
            ss << ", ";
        }
    }
    ss << ")";

    if (expr.return_type) {
        ss << " -> " << transpileType(*expr.return_type);
    }

    ss << " ";
    // Temporarily redirect 'out' to capture the block's transpilation
    std::stringstream body_out;
    std::swap(out, body_out);

    visitBlockStmt(*expr.body, false);

    std::swap(out, body_out); // Restore original 'out'

    ss << body_out.str();

    return ss.str();
}
std::any Transpiler::visitGetExpr(const GetExpr& expr) {
    if (auto var_expr = dynamic_cast<const VariableExpr*>(expr.object.get())) {
        if (var_expr->name.lexeme == "math") {
            if (expr.name.lexeme == "PI") {
                return std::string("M_PI");
            }
            if (expr.name.lexeme == "E") {
                return std::string("M_E");
            }
        }
    }

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

#include <variant>

// ... (other includes)

std::any Transpiler::visitStructLiteralExpr(const StructLiteralExpr& expr) {
    std::stringstream out;
    out << expr.name.lexeme << "{";

    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::map<std::string, std::unique_ptr<Expr>>>) {
            // Named fields
            bool first = true;
            for (const auto& [name, value] : arg) {
                if (!first) {
                    out << ", ";
                }
                first = false;
                out << "." << name << " = " << std::any_cast<std::string>(value->accept(*this));
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::unique_ptr<Expr>>>) {
            // Positional fields
            bool first = true;
            for (const auto& value : arg) {
                if (!first) {
                    out << ", ";
                }
                first = false;
                out << std::any_cast<std::string>(value->accept(*this));
            }
        }
    }, expr.initializers);

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
    return "static_cast<" + transpileType(*expr.target_type) + ">(" + std::any_cast<std::string>(expr.expression->accept(*this)) + ")";
}

std::any Transpiler::visitBlockStmt(const BlockStmt& stmt) {
    visitBlockStmt(stmt, true);
    return nullptr;
}

void Transpiler::visitBlockStmt(const BlockStmt& stmt, bool create_scope) {
    out << "{\n";
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    out << "}\n";
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

    out << (type.is_mutable ? "" : "const ") << getTypeString(type) << " " << stmt.name.lexeme;
    if (stmt.initializer) {
        TypeInfo old_context = contextual_type;
        contextual_type = type;
        out << " = ";
        if (type.is_ref) {
            if (auto borrow_expr = dynamic_cast<const BorrowExpr*>(stmt.initializer.get())) {
                out << std::any_cast<std::string>(borrow_expr->expression->accept(*this));
            } else {
                out << std::any_cast<std::string>(stmt.initializer->accept(*this));
            }
        } else {
            out << std::any_cast<std::string>(stmt.initializer->accept(*this));
        }
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
    // Check for for-in loop (simplified check)
    if (stmt.initializer && stmt.condition && !stmt.increment) { // condition holds the iterable
        if (auto* varStmt = dynamic_cast<VarStmt*>(stmt.initializer.get())) {
            std::string iterable_name = std::any_cast<std::string>(stmt.condition->accept(*this));
            out << "for (const auto& " << varStmt->name.lexeme << " : " << iterable_name << ") ";
            stmt.body->accept(*this);
            return nullptr;
        }
    }

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
    } else if (std::holds_alternative<std::string>(stmt.path)) {
        std::string file_path_str = std::get<std::string>(stmt.path);
        std::filesystem::path file_path(file_path_str);
        if (!std::filesystem::exists(file_path)) {
            // In a real compiler, we'd report a file not found error.
            return nullptr;
        }
        std::string canonical_path = std::filesystem::canonical(file_path).u8string();

        if (transpiled_files->count(canonical_path)) {
            return nullptr; // Already transpiled
        }
        transpiled_files->insert(canonical_path);

        std::ifstream file(canonical_path);
        if (!file.is_open()) {
            // In a real compiler, we'd report a file not found error.
            return nullptr;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string file_content = buffer.str();

        Lexer lexer(file_content);
        std::vector<Token> tokens = lexer.scanTokens();
        Parser parser(tokens);
        std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

        // Pre-scan for declarations in the imported file
        DeclarationScanner scanner;
        for (const auto& statement : statements) {
            statement->accept(scanner);
        }
        // Merge declarations into the current transpiler
        this->structs.insert(scanner.structs.begin(), scanner.structs.end());
        this->enums.insert(scanner.enums.begin(), scanner.enums.end());

        // Transpile the imported statements into a temporary stream
        std::stringstream imported_out;
        std::swap(out, imported_out);

        for (const auto& statement : statements) {
            statement->accept(*this);
        }

        // Restore the original stream and prepend the imported code
        std::swap(out, imported_out);
        std::string original_content = out.str();
        out.str("");
        out << imported_out.str() << original_content;
    }
    return nullptr;
}
std::any Transpiler::visitSwitchStmt(const SwitchStmt& stmt) {
    out << "switch (" << std::any_cast<std::string>(stmt.expression->accept(*this)) << ") {\n";
    for (const auto& case_stmt : stmt.cases) {
        case_stmt->accept(*this);
    }
    out << "}\n";
    return nullptr;
}
std::any Transpiler::visitCaseStmt(const CaseStmt& stmt) {
    if (stmt.value) {
        out << "case " << std::any_cast<std::string>(stmt.value->accept(*this)) << ":\n";
    } else {
        out << "default:\n";
    }
    stmt.body->accept(*this);
    if (auto block = dynamic_cast<const BlockStmt*>(stmt.body.get())) {
        bool has_fallthrough = false;
        bool has_break = false;
        for (const auto& s : block->statements) {
            if (dynamic_cast<const FallthroughStmt*>(s.get())) {
                has_fallthrough = true;
                break;
            }
            if (dynamic_cast<const BreakStmt*>(s.get())) {
                has_break = true;
                break;
            }
        }
        if (!has_fallthrough && !has_break) {
            out << "break;\n";
        }
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

void Transpiler::transpile_function_body(const BlockStmt* body, const std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>>& constraints) {
    if (!body) {
        out << ";\n";
        return;
    }

    out << "{\n";
    transpile_generic_constraints(constraints);

    // Temporarily redirect 'out' to capture the block's content
    std::stringstream body_out;
    std::swap(out, body_out);

    for (const auto& statement : body->statements) {
        statement->accept(*this);
    }

    std::swap(out, body_out); // Restore original 'out'
    out << body_out.str() << "}\n";
}


void Transpiler::transpile_generic_constraints(const std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>>& constraints) {
    if (constraints.empty()) {
        return;
    }

    type_traits_used = true;
    for (const auto& [generic_param, traits] : constraints) {
        for (const auto& trait : traits) {
            std::string trait_name = transpileType(*trait);
            out << "static_assert(std::is_base_of<" << trait_name << ", " << generic_param << ">::value, "
                << "\"" << generic_param << " must implement " << trait_name << "\");\n";
        }
    }
}

std::string Transpiler::transpileType(const TypeExpr& type) {
    if (auto baseType = dynamic_cast<const BaseTypeExpr*>(&type)) {
        if (baseType->type.lexeme == "option") {
            optional_used = true;
            return "std::optional";
        }
        if (baseType->type.lexeme == "result") {
            result_used = true;
            return "result";
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
            base_name = "result";
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
    if (auto funcType = dynamic_cast<const FunctionTypeExpr*>(&type)) {
        function_used = true;
        std::string s = "std::function<";
        s += funcType->return_type ? transpileType(*funcType->return_type) : "void";
        s += "(";
        for (size_t i = 0; i < funcType->param_types.size(); ++i) {
            s += transpileType(*funcType->param_types[i]);
            if (i < funcType->param_types.size() - 1) {
                s += ", ";
            }
        }
        s += ")>";
        return s;
    }
    return "auto";
}

std::any Transpiler::visitFunctionStmt(const FunctionStmt& stmt) {
    auto old_return_type = current_function_return_type;
    current_function_return_type = stmt.return_type.get();

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

    std::string function_name = stmt.name.lexeme;
    bool is_chtholly_main = is_main_file && function_name == "main";
    if (is_chtholly_main) {
        function_name = "chtholly_main";
    }

    out << (stmt.return_type ? transpileType(*stmt.return_type) : "void") << " " << function_name << "(";

    if (is_chtholly_main) {
        out << "std::vector<std::string> args";
        vector_used = true;
        string_used = true;
    } else {
        bool first_param = true;
        for (size_t i = 0; i < stmt.params.size(); ++i) {
            if (stmt.params[i].lexeme == "self") continue;
            if (!first_param) out << ", ";
            first_param = false;

            TypeInfo paramType = typeExprToTypeInfo(stmt.param_types[i].get());
            out << getTypeString(paramType) << " " << stmt.params[i].lexeme;
        }
    }
    out << ") ";
    transpile_function_body(stmt.body.get(), stmt.generic_constraints);

    current_function_return_type = old_return_type;
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
    out << "struct " << stmt.name.lexeme;

    std::set<std::string> inherited_traits;
    for (const auto& method : stmt.methods) {
        if (method->trait_impl) {
            if (auto* type_expr = dynamic_cast<TypeExpr*>(method->trait_impl.get())) {
                inherited_traits.insert(transpileType(*type_expr));
            }
        }
    }

    for (const auto& trait : stmt.traits) {
         if (auto* type_expr = dynamic_cast<TypeExpr*>(trait.get())) {
            inherited_traits.insert(transpileType(*type_expr));
        }
    }

    if (!inherited_traits.empty()) {
        out << " : ";
        bool first = true;
        for (const auto& trait : inherited_traits) {
            if (!first) {
                out << ", ";
            }
            out << "public " << trait;
            first = false;
        }
    }

    out << " {\n";

    std::vector<const VarStmt*> public_fields;
    std::vector<const VarStmt*> private_fields;
    for (const auto& field : stmt.fields) {
        if (field->access == Access::PUBLIC) {
            public_fields.push_back(field.get());
        } else {
            private_fields.push_back(field.get());
        }
    }

    std::vector<const FunctionStmt*> public_methods;
    std::vector<const FunctionStmt*> private_methods;
    for (const auto& method : stmt.methods) {
        if (method->access == Access::PUBLIC) {
            public_methods.push_back(method.get());
        } else {
            private_methods.push_back(method.get());
        }
    }

    if (!public_fields.empty() || !public_methods.empty()) {
        out << "public:\n";
        for (const auto& field : public_fields) {
            out << transpileType(*field->type) << " " << field->name.lexeme;
            if (field->initializer) {
                out << " = " << std::any_cast<std::string>(field->initializer->accept(*this));
            }
            out << ";\n";
        }
        for (const auto& method : public_methods) {
            is_in_method = true;
            current_struct = &stmt;
            out << (method->return_type ? transpileType(*method->return_type) : "void") << " " << method->name.lexeme << "(";
            bool first_param = true;
            for (size_t i = 0; i < method->params.size(); ++i) {
                if (method->params[i].lexeme == "self") continue;
                if (!first_param) out << ", ";
                first_param = false;
                out << transpileType(*method->param_types[i]) << " " << method->params[i].lexeme;
            }
            out << ") ";
            if (method->trait_impl) {
                out << "override ";
            }
            transpile_function_body(method->body.get(), method->generic_constraints);
            is_in_method = false;
            current_struct = nullptr;
        }
    }

    if (!private_fields.empty() || !private_methods.empty()) {
        out << "private:\n";
        for (const auto& field : private_fields) {
            out << transpileType(*field->type) << " " << field->name.lexeme;
            if (field->initializer) {
                out << " = " << std::any_cast<std::string>(field->initializer->accept(*this));
            }
            out << ";\n";
        }
        for (const auto& method : private_methods) {
            is_in_method = true;
            current_struct = &stmt;
            out << (method->return_type ? transpileType(*method->return_type) : "void") << " " << method->name.lexeme << "(";
            bool first_param = true;
            for (size_t i = 0; i < method->params.size(); ++i) {
                if (method->params[i].lexeme == "self") continue;
                if (!first_param) out << ", ";
                first_param = false;
                out << transpileType(*method->param_types[i]) << " " << method->params[i].lexeme;
            }
            out << ") ";
            if (method->trait_impl) {
                out << "override ";
            }
            transpile_function_body(method->body.get(), method->generic_constraints);
            is_in_method = false;
            current_struct = nullptr;
        }
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
