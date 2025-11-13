#include "Resolver.h"
#include "AST.h"
#include <algorithm>

namespace chtholly {

class Resolver::DeclarationScanner : public StmtVisitor, public ExprVisitor {
public:
    explicit DeclarationScanner(Resolver& resolver) : resolver(resolver) {}

    std::any visitStructStmt(const StructStmt& stmt) override {
        resolver.structs[stmt.name.lexeme] = &stmt;
        return nullptr;
    }
    std::any visitEnumStmt(const EnumStmt& stmt) override {
        resolver.enums[stmt.name.lexeme] = &stmt;
        return nullptr;
    }
    std::any visitTraitStmt(const TraitStmt& stmt) override {
        resolver.traits[stmt.name.lexeme] = &stmt;
        return nullptr;
    }
    std::any visitBlockStmt(const BlockStmt& stmt) override { for (const auto& s : stmt.statements) s->accept(*this); return nullptr; }
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override { stmt.expression->accept(*this); return nullptr; }
    std::any visitFunctionStmt(const FunctionStmt& stmt) override {
        resolver.functions[stmt.name.lexeme] = &stmt;
        return nullptr;
    }
    std::any visitIfStmt(const IfStmt& stmt) override { stmt.thenBranch->accept(*this); if (stmt.elseBranch) stmt.elseBranch->accept(*this); return nullptr; }
    std::any visitVarStmt(const VarStmt& stmt) override { if (stmt.initializer) stmt.initializer->accept(*this); return nullptr; }
    std::any visitWhileStmt(const WhileStmt& stmt) override { stmt.body->accept(*this); return nullptr; }
    std::any visitForStmt(const ForStmt& stmt) override { stmt.body->accept(*this); return nullptr; }
    std::any visitReturnStmt(const ReturnStmt& stmt) override { return nullptr; }
    std::any visitImportStmt(const ImportStmt& stmt) override { return nullptr; }
    std::any visitSwitchStmt(const SwitchStmt& stmt) override { for (const auto& c : stmt.cases) c->accept(*this); return nullptr; }
    std::any visitCaseStmt(const CaseStmt& stmt) override { stmt.body->accept(*this); return nullptr; }
    std::any visitBreakStmt(const BreakStmt& stmt) override { return nullptr; }
    std::any visitFallthroughStmt(const FallthroughStmt& stmt) override { return nullptr; }
    std::any visitBinaryExpr(const BinaryExpr& expr) override { expr.left->accept(*this); expr.right->accept(*this); return nullptr; }
    std::any visitUnaryExpr(const UnaryExpr& expr) override { expr.right->accept(*this); return nullptr; }
    std::any visitLiteralExpr(const LiteralExpr& expr) override { return nullptr; }
    std::any visitGroupingExpr(const GroupingExpr& expr) override { expr.expression->accept(*this); return nullptr; }
    std::any visitVariableExpr(const VariableExpr& expr) override { return nullptr; }
    std::any visitAssignExpr(const AssignExpr& expr) override { expr.value->accept(*this); return nullptr; }
    std::any visitCallExpr(const CallExpr& expr) override { expr.callee->accept(*this); for (const auto& arg : expr.arguments) arg->accept(*this); return nullptr; }
    std::any visitLambdaExpr(const LambdaExpr& expr) override { return nullptr; }
    std::any visitGetExpr(const GetExpr& expr) override { expr.object->accept(*this); return nullptr; }
    std::any visitSetExpr(const SetExpr& expr) override { expr.object->accept(*this); expr.value->accept(*this); return nullptr; }
    std::any visitSelfExpr(const SelfExpr& expr) override { return nullptr; }
    std::any visitBorrowExpr(const BorrowExpr& expr) override { return nullptr; }
    std::any visitDerefExpr(const DerefExpr& expr) override { return nullptr; }
    std::any visitStructLiteralExpr(const StructLiteralExpr& expr) override { return nullptr; }
    std::any visitArrayLiteralExpr(const ArrayLiteralExpr& expr) override { return nullptr; }
    std::any visitTypeCastExpr(const TypeCastExpr& expr) override { return nullptr; }

private:
    Resolver& resolver;
};

const std::map<TokenType, std::string> Resolver::op_to_trait = {
    {TokenType::PLUS, "Add"},
    {TokenType::MINUS, "Subtract"},
    {TokenType::STAR, "Multiply"},
    {TokenType::SLASH, "Divide"},
    {TokenType::EQUAL_EQUAL, "Equal"},
    {TokenType::BANG_EQUAL, "Equal"},
    {TokenType::LESS, "Less"},
    {TokenType::LESS_EQUAL, "Less"},
    {TokenType::GREATER, "Greater"},
    {TokenType::GREATER_EQUAL, "Greater"},
};

Resolver::Resolver() {
    // Pre-define all built-in functions and modules
    auto any_type = std::make_shared<BasicType>("any");
    auto void_type = std::make_shared<BasicType>("void");
    symbols.define("print", std::make_shared<chtholly::FunctionType>(void_type, std::vector<std::shared_ptr<Type>>{any_type}));
    symbols.define("input", std::make_shared<chtholly::FunctionType>(std::make_shared<BasicType>("string"), std::vector<std::shared_ptr<Type>>{}));
    symbols.define("fs_read", std::make_shared<chtholly::FunctionType>(std::make_shared<BasicType>("string"), std::vector<std::shared_ptr<Type>>{std::make_shared<BasicType>("string")}));
    symbols.define("fs_write", any_type);
    symbols.define("fs_exists", any_type);
    symbols.define("fs_is_file", any_type);
    symbols.define("fs_is_dir", any_type);
    symbols.define("fs_list_dir", any_type);
    symbols.define("fs_remove", any_type);
    symbols.define("meta", any_type);
    symbols.define("reflect", any_type);
    symbols.define("util", any_type);
    symbols.define("operator", any_type);
    symbols.define("string", any_type);
    symbols.define("array", any_type);
    symbols.define("os", any_type);
    symbols.define("time", any_type);
    symbols.define("random", any_type);
    symbols.define("math", any_type);

    // Pre-define reflect types
    symbols.define("Field", std::make_shared<StructType>("Field"));
    symbols.define("Method", std::make_shared<StructType>("Method"));
    symbols.define("Trait", std::make_shared<StructType>("Trait"));

    // To allow `Resolver` to treat them as valid structs
    structs["Field"] = nullptr;
    structs["Method"] = nullptr;
    structs["Trait"] = nullptr;
}

void Resolver::error(const Token& token, const std::string& message) {
    hadError = true;
    std::cerr << "[line " << token.line << "] Error";
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end";
    } else {
        std::cerr << " at '" << token.lexeme << "'";
    }
    std::cerr << ": " << message << std::endl;
}

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    DeclarationScanner scanner(*this);
    for (const auto& statement : statements) {
        statement->accept(scanner);
    }

    for (const auto& statement : statements) {
        resolve(*statement);
    }
}

void Resolver::resolve(const Stmt& stmt) {
    stmt.accept(*this);
}

void Resolver::resolve(const Expr& expr) {
    expr.accept(*this);
}

std::any Resolver::visitBlockStmt(const BlockStmt& stmt) {
    symbols.enter_scope();
    for (const auto& statement : stmt.statements) {
        resolve(*statement);
    }
    symbols.exit_scope();
    return nullptr;
}

std::any Resolver::visitVarStmt(const VarStmt& stmt) {
    if (symbols.is_defined_in_current_scope(stmt.name.lexeme)) {
        error(stmt.name, "Already a variable with this name in this scope.");
    }

    std::shared_ptr<Type> value_type = nullptr;
    if (stmt.initializer) {
        resolve(*stmt.initializer);
        value_type = stmt.initializer->type;
    }

    if (stmt.type) {
        auto declared_type = resolveTypeExpr(*stmt.type);
        if (value_type && declared_type && !value_type->equals(*declared_type)) {
            error(stmt.name, "Initializer type '" + value_type->to_string() +
                               "' does not match declared type '" + declared_type->to_string() + "'.");
        }
        value_type = declared_type;
    }

    symbols.define(stmt.name.lexeme, value_type);
    return nullptr;
}

std::any Resolver::visitVariableExpr(const VariableExpr& expr) {
    auto type = symbols.resolve(expr.name);
    if (type) {
        expr.type = type;
    } else {
        error(expr.name, "Undefined variable.");
    }
    return nullptr;
}

std::shared_ptr<Type> Resolver::resolveTypeExpr(const TypeExpr& type_expr) {
    return type_expr.accept(*this);
}

std::shared_ptr<Type> Resolver::visitBaseTypeExpr(const BaseTypeExpr& expr) {
    std::string name = expr.type.lexeme;
    if (name == "int" || name == "double" || name == "string" || name == "bool" || name == "char") {
        return std::make_shared<BasicType>(name);
    }
    auto it = structs.find(name);
    if (it != structs.end()) {
        return std::make_shared<StructType>(name);
    }
    error(expr.type, "Unknown type name '" + name + "'.");
    return nullptr;
}

std::shared_ptr<Type> Resolver::visitArrayTypeExpr(const ArrayTypeExpr& expr) {
    auto element_type = resolveTypeExpr(*expr.element_type);
    if (!element_type) {
        return nullptr;
    }
    return std::make_shared<ArrayType>(element_type);
}

std::shared_ptr<Type> Resolver::visitFunctionTypeExpr(const FunctionTypeExpr& expr) {
    error(Token(TokenType::FUNCTION, "function", nullptr, 0), "Function types are not yet supported in type annotations.");
    return std::make_shared<BasicType>("unsupported");
}

std::shared_ptr<Type> Resolver::visitGenericTypeExpr(const GenericTypeExpr& expr) {
    error(expr.base_type, "Generic types are not yet supported in type annotations.");
    return std::make_shared<BasicType>("unsupported");
}

std::shared_ptr<Type> Resolver::visitBorrowTypeExpr(const BorrowTypeExpr& expr) {
    auto element_type = resolveTypeExpr(*expr.element_type);
    if (!element_type) {
        return nullptr;
    }
    return std::make_shared<BorrowType>(element_type, expr.isMutable);
}

std::any Resolver::visitAssignExpr(const AssignExpr& expr) {
    resolve(*expr.value);
    auto value_type = expr.value->type;
    auto var_type = symbols.resolve(expr.name);

    if (!var_type) {
        error(expr.name, "Undefined variable.");
        return nullptr;
    }

    if (value_type && var_type && !value_type->equals(*var_type)) {
        error(expr.name, "Type mismatch. Cannot assign value of type '" + value_type->to_string() +
                           "' to variable of type '" + var_type->to_string() + "'.");
    }

    return nullptr;
}

void Resolver::resolveFunction(const FunctionStmt& function, CurrentFunctionType type) {
    CurrentFunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    auto enclosing_return_type = std::move(current_return_type);
    current_return_type = function.return_type ? resolveTypeExpr(*function.return_type) : std::make_shared<BasicType>("void");

    symbols.enter_scope();
    for (size_t i = 0; i < function.params.size(); ++i) {
        auto param_type = resolveTypeExpr(*function.param_types[i]);
        symbols.define(function.params[i].lexeme, param_type);
    }
    resolve(*function.body);
    symbols.exit_scope();
    currentFunction = enclosingFunction;
    current_return_type = std::move(enclosing_return_type);
}

std::any Resolver::visitFunctionStmt(const FunctionStmt& stmt) {
    if (symbols.is_defined_in_current_scope(stmt.name.lexeme)) {
        error(stmt.name, "Already a variable with this name in this scope.");
    }

    std::vector<std::shared_ptr<Type>> param_types;
    for (const auto& param_type_expr : stmt.param_types) {
        param_types.push_back(resolveTypeExpr(*param_type_expr));
    }
    auto return_type = stmt.return_type ? resolveTypeExpr(*stmt.return_type) : std::make_shared<BasicType>("void");
    auto func_type = std::make_shared<chtholly::FunctionType>(return_type, param_types);
    symbols.define(stmt.name.lexeme, func_type);

    resolveFunction(stmt, CurrentFunctionType::FUNCTION);
    return nullptr;
}

std::any Resolver::visitExpressionStmt(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
    return nullptr;
}

std::any Resolver::visitIfStmt(const IfStmt& stmt) {
    resolve(*stmt.condition);
    if (stmt.condition->type && !stmt.condition->type->equals(BasicType("bool"))) {
        // Ideally, we'd have a token from the expression to report the error on.
        // For now, the message is clear enough.
        error(Token(TokenType::ERROR, "", nullptr, 0), "If condition must be of type bool.");
    }
    resolve(*stmt.thenBranch);
    if (stmt.elseBranch) {
        resolve(*stmt.elseBranch);
    }
    return nullptr;
}

std::any Resolver::visitReturnStmt(const ReturnStmt& stmt) {
    if (currentFunction == CurrentFunctionType::NONE) {
        error(stmt.keyword, "Can't return from top-level code.");
    }

    if (stmt.value) {
        if (current_return_type && current_return_type->equals(BasicType("void"))) {
            error(stmt.keyword, "Cannot return a value from a void function.");
        }
        resolve(*stmt.value);
        if (current_return_type && stmt.value->type &&
            !current_return_type->equals(*stmt.value->type)) {
            error(stmt.keyword, "Return type mismatch. Expected " + current_return_type->to_string() +
                                 " but got " + stmt.value->type->to_string() + ".");
        }
    } else {
        if (current_return_type && !current_return_type->equals(BasicType("void"))) {
            error(stmt.keyword, "Non-void function must return a value.");
        }
    }

    return nullptr;
}

std::any Resolver::visitWhileStmt(const WhileStmt& stmt) {
    resolve(*stmt.condition);
    if (stmt.condition->type && !stmt.condition->type->equals(BasicType("bool"))) {
        error(Token(TokenType::ERROR, "", nullptr, 0), "While condition must be of type bool.");
    }

    LoopType enclosingLoop = currentLoop;
    currentLoop = LoopType::LOOP;
    resolve(*stmt.body);
    currentLoop = enclosingLoop;

    return nullptr;
}

std::any Resolver::visitForStmt(const ForStmt& stmt) {
    if (stmt.initializer) {
        resolve(*stmt.initializer);
    }
    if (stmt.condition) {
        resolve(*stmt.condition);
        if (stmt.condition->type && !stmt.condition->type->equals(BasicType("bool"))) {
            error(Token(TokenType::ERROR, "", nullptr, 0), "For condition must be of type bool.");
        }
    }
    if (stmt.increment) {
        resolve(*stmt.increment);
    }

    LoopType enclosingLoop = currentLoop;
    currentLoop = LoopType::LOOP;
    resolve(*stmt.body);
    currentLoop = enclosingLoop;

    return nullptr;
}

bool Resolver::is_trait_implemented(const std::string& struct_name, const std::string& trait_name) {
    auto it = structs.find(struct_name);
    if (it == structs.end()) {
        return false; // Struct not found
    }
    const StructStmt* struct_stmt = it->second;
    for (const auto& trait_expr : struct_stmt->traits) {
        if (auto base_type_expr = dynamic_cast<const BaseTypeExpr*>(trait_expr.get())) {
            if (base_type_expr->type.lexeme == trait_name) {
                return true;
            }
        }
    }
    return false;
}

std::any Resolver::visitBinaryExpr(const BinaryExpr& expr) {
    resolve(*expr.left);
    resolve(*expr.right);

    if (!expr.left->type || !expr.right->type) {
        return nullptr; // Can't check types if one is missing
    }

    if (expr.left->type->get_kind() != TypeKind::BASIC || expr.right->type->get_kind() != TypeKind::BASIC) {
        // For now, only handle binary ops on basic types. Structs would use traits.
        // This is a simplification and might need to be expanded.
        error(expr.op, "Operator " + expr.op.lexeme + " cannot be applied to non-basic types yet.");
        return nullptr;
    }

    // Type checking for operands
    auto left_type = expr.left->type;
    auto right_type = expr.right->type;

    switch (expr.op.type) {
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::BANG_EQUAL:
        case TokenType::EQUAL_EQUAL:
            if (!((left_type->equals(BasicType("int")) || left_type->equals(BasicType("double"))) &&
                  (right_type->equals(BasicType("int")) || right_type->equals(BasicType("double"))))) {
                 error(expr.op, "Comparison operators can only be applied to numbers.");
            }
            expr.type = std::make_shared<BasicType>("bool");
            break;

        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::STAR:
        case TokenType::SLASH:
             if (!((left_type->equals(BasicType("int")) || left_type->equals(BasicType("double"))) &&
                   (right_type->equals(BasicType("int")) || right_type->equals(BasicType("double"))))) {
                 error(expr.op, "Arithmetic operators can only be applied to numbers.");
            }
            if (left_type->equals(BasicType("double")) || right_type->equals(BasicType("double"))) {
                expr.type = std::make_shared<BasicType>("double");
            } else {
                expr.type = std::make_shared<BasicType>("int");
            }
            break;

        case TokenType::AND:
        case TokenType::OR:
            if (!left_type->equals(BasicType("bool")) || !right_type->equals(BasicType("bool"))) {
                error(expr.op, "Logical operators can only be applied to booleans.");
            }
            expr.type = std::make_shared<BasicType>("bool");
            break;

        default:
             error(expr.op, "Unsupported binary operator.");
            break;
    }

    return nullptr;
}

std::any Resolver::visitCallExpr(const CallExpr& expr) {
    resolve(*expr.callee);
    for (const auto& arg : expr.arguments) {
        resolve(*arg);
    }

    if (auto get_expr = dynamic_cast<const GetExpr*>(expr.callee.get())) {
        if (auto var_expr = dynamic_cast<const VariableExpr*>(get_expr->object.get())) {
            if (var_expr->name.lexeme == "meta") {
                // This is a call to a meta function
                std::string func_name = get_expr->name.lexeme;

                // List of functions that return bool
                std::vector<std::string> bool_funcs = {
                    "is_int", "is_uint", "is_double", "is_char", "is_bool",
                    "is_string", "is_struct", "is_array", "is_let", "is_mut",
                    "is_borrow", "is_borrow_mut", "is_move"
                };

                bool is_bool_func = false;
                for (const auto& name : bool_funcs) {
                    if (func_name == name) {
                        is_bool_func = true;
                        break;
                    }
                }

                if (is_bool_func) {
                    if (expr.arguments.size() != 1) {
                        error(get_expr->name, "meta::" + func_name + " requires 1 argument.");
                    }
                    expr.type = std::make_shared<BasicType>("bool");
                    return nullptr;
                }

                if (func_name == "type_name") {
                    if (expr.arguments.size() != 1) {
                        error(get_expr->name, "meta::type_name requires 1 argument.");
                    }
                    expr.type = std::make_shared<BasicType>("string");
                    return nullptr;
                }

                error(get_expr->name, "Unknown function meta::" + func_name);
                return nullptr;
            } else if (var_expr->name.lexeme == "reflect") {
                std::string func_name = get_expr->name.lexeme;

                if (func_name == "get_field_count" || func_name == "get_method_count" || func_name == "get_trait_count") {
                    if (expr.arguments.size() != 1) {
                        error(get_expr->name, "reflect::" + func_name + " requires 1 argument.");
                        return nullptr;
                    }
                    if (expr.arguments[0]->type->get_kind() != TypeKind::STRUCT) {
                        error(get_expr->name, "reflect::" + func_name + " requires a struct type as an argument.");
                    }
                    expr.type = std::make_shared<BasicType>("int");
                    return nullptr;
                }

                if (func_name == "get_fields" || func_name == "get_methods" || func_name == "get_traits") {
                    if (expr.arguments.size() != 1) {
                        error(get_expr->name, "reflect::" + func_name + " requires 1 argument.");
                        return nullptr;
                    }
                    if (expr.arguments[0]->type->get_kind() != TypeKind::STRUCT) {
                        error(get_expr->name, "reflect::" + func_name + " requires a struct type as an argument.");
                    }

                    if (func_name == "get_fields") expr.type = std::make_shared<ArrayType>(std::make_shared<StructType>("Field"));
                    if (func_name == "get_methods") expr.type = std::make_shared<ArrayType>(std::make_shared<StructType>("Method"));
                    if (func_name == "get_traits") expr.type = std::make_shared<ArrayType>(std::make_shared<StructType>("Trait"));
                    return nullptr;
                }

                if (func_name == "get_field" || func_name == "get_method" || func_name == "get_trait") {
                    if (expr.arguments.size() != 2) {
                        error(get_expr->name, "reflect::" + func_name + " requires 2 arguments.");
                        return nullptr;
                    }
                    if (expr.arguments[0]->type->get_kind() != TypeKind::STRUCT) {
                        error(get_expr->name, "reflect::" + func_name + " requires a struct type as the first argument.");
                    }
                    if (expr.arguments[1]->type && !expr.arguments[1]->type->equals(BasicType("string"))) {
                        error(get_expr->name, "reflect::" + func_name + " requires a string as the second argument.");
                    }

                    if (func_name == "get_field") expr.type = std::make_shared<StructType>("Field");
                    if (func_name == "get_method") expr.type = std::make_shared<StructType>("Method");
                    if (func_name == "get_trait") expr.type = std::make_shared<StructType>("Trait");
                    return nullptr;
                }

                error(get_expr->name, "Unknown function reflect::" + func_name);
                return nullptr;
            } else if (var_expr->name.lexeme == "os") {
                std::string func_name = get_expr->name.lexeme;
                if (func_name == "exit") {
                    if (expr.arguments.size() != 1) {
                        error(get_expr->name, "os::exit requires 1 argument.");
                        return nullptr;
                    }
                    if (expr.arguments[0]->type && !expr.arguments[0]->type->equals(BasicType("int"))) {
                        error(get_expr->name, "os::exit requires an integer argument.");
                    }
                    expr.type = std::make_shared<BasicType>("void");
                    return nullptr;
                }
                if (func_name == "env") {
                    if (expr.arguments.size() != 1) {
                        error(get_expr->name, "os::env requires 1 argument.");
                        return nullptr;
                    }
                    if (expr.arguments[0]->type && !expr.arguments[0]->type->equals(BasicType("string"))) {
                        error(get_expr->name, "os::env requires a string argument.");
                    }
                    // This is a simplification. A full implementation would create a GenericType.
                    expr.type = std::make_shared<StructType>("option");
                    return nullptr;
                }
                error(get_expr->name, "Unknown function os::" + func_name);
                return nullptr;
            } else if (var_expr->name.lexeme == "time") {
                std::string func_name = get_expr->name.lexeme;
                if (func_name == "now") {
                    if (expr.arguments.size() != 0) {
                        error(get_expr->name, "time::now requires 0 arguments.");
                    }
                    expr.type = std::make_shared<BasicType>("int"); // Representing long long as int for now
                    return nullptr;
                }
                error(get_expr->name, "Unknown function time::" + func_name);
                return nullptr;
            } else if (var_expr->name.lexeme == "random") {
                std::string func_name = get_expr->name.lexeme;
                if (func_name == "rand") {
                    if (expr.arguments.size() != 0) {
                        error(get_expr->name, "random::rand requires 0 arguments.");
                    }
                    expr.type = std::make_shared<BasicType>("double");
                    return nullptr;
                }
                if (func_name == "randint") {
                    if (expr.arguments.size() != 2) {
                        error(get_expr->name, "random::randint requires 2 arguments.");
                        return nullptr;
                    }
                    if (expr.arguments[0]->type && !expr.arguments[0]->type->equals(BasicType("int"))) {
                        error(get_expr->name, "random::randint first argument must be an integer.");
                    }
                    if (expr.arguments[1]->type && !expr.arguments[1]->type->equals(BasicType("int"))) {
                        error(get_expr->name, "random::randint second argument must be an integer.");
                    }
                    expr.type = std::make_shared<BasicType>("int");
                    return nullptr;
                }
                error(get_expr->name, "Unknown function random::" + func_name);
                return nullptr;
            } else if (var_expr->name.lexeme == "util") {
                std::string func_name = get_expr->name.lexeme;
                if (func_name == "panic") {
                    if (expr.arguments.size() != 1) {
                        error(get_expr->name, "util::panic requires 1 argument.");
                    }
                    expr.type = std::make_shared<BasicType>("void");
                    return nullptr;
                }
                if (func_name == "string_cast") {
                     if (expr.arguments.size() != 1) {
                        error(get_expr->name, "util::string_cast requires 1 argument.");
                    }
                    expr.type = std::make_shared<BasicType>("string");
                    return nullptr;
                }
                error(get_expr->name, "Unknown function util::" + func_name);
                return nullptr;
            } else if (var_expr->name.lexeme == "math") {
                std::string func_name = get_expr->name.lexeme;
                const std::vector<std::string> one_arg_funcs = {
                    "sqrt", "sin", "cos", "tan", "log", "log10", "abs", "ceil", "floor", "round"
                };
                const std::vector<std::string> two_arg_funcs = { "pow" };

                bool is_one_arg = std::find(one_arg_funcs.begin(), one_arg_funcs.end(), func_name) != one_arg_funcs.end();
                bool is_two_arg = std::find(two_arg_funcs.begin(), two_arg_funcs.end(), func_name) != two_arg_funcs.end();

                if (!is_one_arg && !is_two_arg) {
                    error(get_expr->name, "Unknown function math::" + func_name);
                    return nullptr;
                }

                if (is_one_arg) {
                    if (expr.arguments.size() != 1) {
                        error(get_expr->name, "math::" + func_name + " requires 1 argument.");
                        return nullptr;
                    }
                    auto arg_type = expr.arguments[0]->type;
                    if (arg_type && !arg_type->equals(BasicType("int")) && !arg_type->equals(BasicType("double"))) {
                        error(get_expr->name, "math::" + func_name + " requires a numeric argument.");
                    }
                }

                if (is_two_arg) {
                    if (expr.arguments.size() != 2) {
                        error(get_expr->name, "math::" + func_name + " requires 2 arguments.");
                        return nullptr;
                    }
                    for (size_t i = 0; i < expr.arguments.size(); ++i) {
                        auto arg_type = expr.arguments[i]->type;
                        if (arg_type && !arg_type->equals(BasicType("int")) && !arg_type->equals(BasicType("double"))) {
                             error(get_expr->name, "Argument " + std::to_string(i+1) + " of math::" + func_name + " must be numeric.");
                        }
                    }
                }

                expr.type = std::make_shared<BasicType>("double");
                return nullptr;
            }
        }

        auto object_type = get_expr->object->type;
        if (object_type) {
            if (object_type->get_kind() == TypeKind::BASIC && std::dynamic_pointer_cast<BasicType>(object_type)->get_name() == "string") {
                // String method call
                std::string method_name = get_expr->name.lexeme;
                const std::vector<std::string> known_string_methods = {
                    "length", "is_empty", "contains", "starts_with", "ends_with",
                    "to_upper", "to_lower", "trim", "replace", "split", "substr", "find"
                };

                if (std::find(known_string_methods.begin(), known_string_methods.end(), method_name) == known_string_methods.end()) {
                    error(get_expr->name, "Unknown string method '" + method_name + "'.");
                    return nullptr;
                }

                if (method_name == "length" || method_name == "is_empty") {
                    if (expr.arguments.size() != 0) error(get_expr->name, "String method '" + method_name + "' requires 0 arguments.");
                    expr.type = std::make_shared<BasicType>("int");
                } else if (method_name == "contains" || method_name == "starts_with" || method_name == "ends_with") {
                    if (expr.arguments.size() != 1) error(get_expr->name, "String method '" + method_name + "' requires 1 argument.");
                    expr.type = std::make_shared<BasicType>("bool");
                } else {
                    // Methods returning string or other types
                    if (method_name == "find") {
                        expr.type = std::make_shared<StructType>("option"); // Simplified
                    } else {
                        expr.type = std::make_shared<BasicType>("string");
                    }
                }
                return nullptr;
            } else if (object_type->get_kind() == TypeKind::ARRAY) {
                // Array method call
                std::string method_name = get_expr->name.lexeme;
                auto array_type = std::dynamic_pointer_cast<ArrayType>(object_type);

                if (method_name == "length" || method_name == "is_empty") {
                     if (expr.arguments.size() != 0) error(get_expr->name, "Array method '" + method_name + "' requires 0 arguments.");
                    expr.type = std::make_shared<BasicType>("int");
                } else if (method_name == "pop") {
                    if (expr.arguments.size() != 0) error(get_expr->name, "Array method 'pop' requires 0 arguments.");
                    expr.type = array_type->get_element_type();
                } else if (method_name == "push") {
                     if (expr.arguments.size() != 1) error(get_expr->name, "Array method 'push' requires 1 argument.");
                     if (expr.arguments[0]->type && !expr.arguments[0]->type->equals(*array_type->get_element_type())) {
                         error(get_expr->name, "Argument type does not match array element type.");
                     }
                } else {
                     error(get_expr->name, "Unknown array method '" + method_name + "'.");
                }
                return nullptr;
            }
        }
    }

    if (auto var_expr = dynamic_cast<const VariableExpr*>(expr.callee.get())) {
        auto it = functions.find(var_expr->name.lexeme);
        if (it != functions.end()) {
            // It's a global function
            const FunctionStmt* func = it->second;
            if (expr.arguments.size() != func->params.size()) {
                error(expr.paren, "Expected " + std::to_string(func->params.size()) +
                                    " arguments but got " + std::to_string(expr.arguments.size()) + ".");
                return nullptr;
            }
            for (size_t i = 0; i < expr.arguments.size(); ++i) {
                if (expr.arguments[i]->type) {
                    auto param_type = resolveTypeExpr(*func->param_types[i]);
                    if (param_type && !expr.arguments[i]->type->equals(*param_type)) {
                        error(expr.paren, "Argument " + std::to_string(i + 1) + " type mismatch.");
                    }
                }
            }
            if (func->return_type) {
                expr.type = resolveTypeExpr(*func->return_type);
            }
            return nullptr;
        }
    }

    if (expr.callee->type) {
        if (auto func_type = std::dynamic_pointer_cast<chtholly::FunctionType>(expr.callee->type)) {
            if (expr.arguments.size() != func_type->get_parameter_types().size()) {
                 error(expr.paren, "Expected " + std::to_string(func_type->get_parameter_types().size()) +
                                     " arguments but got " + std::to_string(expr.arguments.size()) + ".");
            }
            // Further type checking can be added here
            expr.type = func_type->get_return_type();
        } else {
            error(expr.paren, "Can only call functions and classes.");
        }
    } else if (auto var_expr = dynamic_cast<const VariableExpr*>(expr.callee.get())) {
        error(var_expr->name, "Attempt to call a non-function variable.");
    } else {
        error(expr.paren, "Attempt to call a non-function expression.");
    }

    return nullptr;
}

std::any Resolver::visitGroupingExpr(const GroupingExpr& expr) {
    expr.expression->accept(*this);
    return nullptr;
}

std::any Resolver::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<long long>(expr.value)) {
        expr.type = std::make_shared<BasicType>("int");
    } else if (std::holds_alternative<double>(expr.value)) {
        expr.type = std::make_shared<BasicType>("double");
    } else if (std::holds_alternative<std::string>(expr.value)) {
        expr.type = std::make_shared<BasicType>("string");
    } else if (std::holds_alternative<bool>(expr.value)) {
        expr.type = std::make_shared<BasicType>("bool");
    } else if (std::holds_alternative<char>(expr.value)) {
        expr.type = std::make_shared<BasicType>("char");
    }
    return nullptr;
}

std::any Resolver::visitUnaryExpr(const UnaryExpr& expr) {
    resolve(*expr.right);

    if (!expr.right->type) {
        return nullptr; // Can't check type if it's missing
    }

    switch (expr.op.type) {
        case TokenType::BANG:
            if (!expr.right->type->equals(BasicType("bool"))) {
                error(expr.op, "Operand of '!' must be of type 'bool'.");
            }
            expr.type = std::make_shared<BasicType>("bool");
            break;
        case TokenType::MINUS:
            if (!expr.right->type->equals(BasicType("int")) && !expr.right->type->equals(BasicType("double"))) {
                error(expr.op, "Operand of '-' must be a number.");
            }
            expr.type = expr.right->type;
            break;
        default:
            // Unreachable
            break;
    }

    return nullptr;
}

std::any Resolver::visitGetExpr(const GetExpr& expr) {
    resolve(*expr.object);

    if (!expr.object->type) {
        return nullptr; // Avoid cascade errors
    }

    if (auto var_expr = dynamic_cast<const VariableExpr*>(expr.object.get())) {
        const std::string& module_name = var_expr->name.lexeme;
        if (module_name == "meta" || module_name == "reflect" || module_name == "util" ||
            module_name == "string" || module_name == "array" || module_name == "os" ||
            module_name == "time" || module_name == "random" || module_name == "math") {
            // This is a module access. The actual function type will be determined
            // in visitCallExpr. We don't assign a type to the GetExpr itself here.
            return nullptr;
        }
    }

    if (auto literal_expr = dynamic_cast<const LiteralExpr*>(expr.object.get())) {
        if (literal_expr->type->get_kind() == TypeKind::BASIC && std::dynamic_pointer_cast<BasicType>(literal_expr->type)->get_name() == "string") {
             // It's a string literal, we can proceed to method call resolution in visitCallExpr
            return nullptr;
        }
    }
     if (auto array_literal_expr = dynamic_cast<const ArrayLiteralExpr*>(expr.object.get())) {
        // It's an array literal, we can proceed to method call resolution in visitCallExpr
        return nullptr;
    }

    if (auto struct_type = std::dynamic_pointer_cast<StructType>(expr.object->type)) {
        auto it = structs.find(struct_type->get_name());
        if (it != structs.end()) {
            const StructStmt* struct_stmt = it->second;
            if (struct_stmt) { // Null check for built-in types like Field
                // Look for a field
                for (const auto& field : struct_stmt->fields) {
                    if (field->name.lexeme == expr.name.lexeme) {
                        expr.type = resolveTypeExpr(*field->type);
                        return nullptr;
                    }
                }
                // Look for a method
                for (const auto& method : struct_stmt->methods) {
                    if (method->name.lexeme == expr.name.lexeme) {
                        std::vector<std::shared_ptr<Type>> param_types;
                        for (const auto& param_type_expr : method->param_types) {
                            param_types.push_back(resolveTypeExpr(*param_type_expr));
                        }
                        auto return_type = method->return_type ? resolveTypeExpr(*method->return_type) : std::make_shared<BasicType>("void");
                        expr.type = std::make_shared<chtholly::FunctionType>(return_type, param_types);
                        return nullptr;
                    }
                }
            }
            error(expr.name, "Undefined property '" + expr.name.lexeme + "'.");
        }
    } else if (expr.object->type->get_kind() == TypeKind::BASIC && std::dynamic_pointer_cast<BasicType>(expr.object->type)->get_name() == "string") {
        // Allow method access on strings
        return nullptr;
    } else if (expr.object->type->get_kind() == TypeKind::ARRAY) {
        // Allow method access on arrays
        return nullptr;
    } else {
        error(expr.name, "Only structs, strings, and arrays have properties.");
    }

    return nullptr;
}

std::any Resolver::visitSetExpr(const SetExpr& expr) {
    resolve(*expr.value);
    resolve(*expr.object);

    if (!expr.object->type) {
        return nullptr; // Avoid cascade errors
    }

    if (auto struct_type = std::dynamic_pointer_cast<StructType>(expr.object->type)) {
        auto it = structs.find(struct_type->get_name());
        if (it != structs.end()) {
            const StructStmt* struct_stmt = it->second;
            if (struct_stmt) { // Null check for built-in types like Field
                for (const auto& field : struct_stmt->fields) {
                    if (field->name.lexeme == expr.name.lexeme) {
                        auto expected_type = resolveTypeExpr(*field->type);
                        if (expr.value->type && expected_type && !expr.value->type->equals(*expected_type)) {
                            error(expr.name, "Type mismatch for field '" + expr.name.lexeme +
                                             "'. Expected " + expected_type->to_string() +
                                             " but got " + expr.value->type->to_string() + ".");
                        }
                        return nullptr;
                    }
                }
            }
            error(expr.name, "Undefined property '" + expr.name.lexeme + "'.");
        }
    } else {
        error(expr.name, "Only structs have fields that can be set.");
    }

    return nullptr;
}

std::any Resolver::visitSelfExpr(const SelfExpr& expr) {
    if (currentClass == ClassType::NONE) {
        error(expr.keyword, "Can't use 'self' outside of a class.");
        return nullptr;
    }

    auto type = symbols.resolve(expr.keyword);
    if (type) {
        expr.type = type;
    } else {
        // This should not happen if currentClass is correctly set
        error(expr.keyword, "Could not resolve 'self'.");
    }

    return nullptr;
}

std::any Resolver::visitStructStmt(const StructStmt& stmt) {
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;

    if (symbols.is_defined_in_current_scope(stmt.name.lexeme)) {
        error(stmt.name, "Already a variable with this name in this scope.");
    }
    symbols.define(stmt.name.lexeme, std::make_shared<StructType>(stmt.name.lexeme));

    symbols.enter_scope();
    symbols.define("self", std::make_shared<StructType>(stmt.name.lexeme));

    for (const auto& method : stmt.methods) {
        CurrentFunctionType declaration = CurrentFunctionType::METHOD;
        resolveFunction(*method, declaration);
    }

    symbols.exit_scope();

    currentClass = enclosingClass;
    return nullptr;
}

std::any Resolver::visitArrayLiteralExpr(const ArrayLiteralExpr& expr) {
    if (expr.elements.empty()) {
        expr.type = std::make_shared<ArrayType>(std::make_shared<BasicType>("any"));
        return nullptr;
    }

    resolve(*expr.elements[0]);
    auto first_type = expr.elements[0]->type;
    if (!first_type) {
        return nullptr; // Cannot determine type
    }

    for (size_t i = 1; i < expr.elements.size(); ++i) {
        resolve(*expr.elements[i]);
        if (expr.elements[i]->type && !first_type->equals(*expr.elements[i]->type)) {
            error(Token(TokenType::ERROR, "", nullptr, 0), "Array elements must have the same type.");
            return nullptr;
        }
    }

    expr.type = std::make_shared<ArrayType>(first_type);
    return nullptr;
}

std::any Resolver::visitTypeCastExpr(const TypeCastExpr& expr) {
    resolve(*expr.expression);
    expr.type = resolveTypeExpr(*expr.target_type);
    return nullptr;
}

std::any Resolver::visitStructLiteralExpr(const StructLiteralExpr& expr) {
    auto it = structs.find(expr.name.lexeme);
    if (it == structs.end()) {
        error(expr.name, "Undefined struct '" + expr.name.lexeme + "'.");
        return nullptr;
    }

    const StructStmt* struct_stmt = it->second;
    expr.type = std::make_shared<StructType>(expr.name.lexeme);

    if (std::holds_alternative<std::map<std::string, std::unique_ptr<Expr>>>(expr.initializers)) {
        const auto& initializers = std::get<std::map<std::string, std::unique_ptr<Expr>>>(expr.initializers);

        if (initializers.size() != struct_stmt->fields.size()) {
            error(expr.name, "Expected " + std::to_string(struct_stmt->fields.size()) +
                               " fields but got " + std::to_string(initializers.size()) + ".");
            return nullptr;
        }

        for (const auto& [name, value] : initializers) {
            resolve(*value);
        }

        for (const auto& field_def : struct_stmt->fields) {
            auto init_it = initializers.find(field_def->name.lexeme);
            if (init_it == initializers.end()) {
                error(expr.name, "Missing field '" + field_def->name.lexeme + "' in initializer.");
                continue;
            }

            auto expected_type = resolveTypeExpr(*field_def->type);
            auto actual_type = init_it->second->type;
            if (actual_type && expected_type && !expected_type->equals(*actual_type)) {
                error(expr.name, "Type mismatch for field '" + field_def->name.lexeme +
                                 "'. Expected " + expected_type->to_string() +
                                 " but got " + actual_type->to_string() + ".");
            }
        }

        for (const auto& [name, value] : initializers) {
            bool found = false;
            for (const auto& field_def : struct_stmt->fields) {
                if (field_def->name.lexeme == name) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                error(expr.name, "Undefined field '" + name + "' in initializer.");
            }
        }
    } else {
        // TODO: Handle positional initializers
    }

    return nullptr;
}

std::any Resolver::visitBorrowExpr(const BorrowExpr& expr) {
    resolve(*expr.expression);
    if (!expr.expression->type) {
        return nullptr; // Avoid cascade errors
    }
    expr.type = std::make_shared<BorrowType>(expr.expression->type, expr.isMutable);
    return nullptr;
}

std::any Resolver::visitSwitchStmt(const SwitchStmt& stmt) {
    resolve(*stmt.expression);
    if (!stmt.expression->type) {
        return nullptr; // Avoid cascade errors
    }

    switch_depth++;
    for (const auto& case_stmt : stmt.cases) {
        resolve(*case_stmt);
        if (auto case_value = dynamic_cast<const CaseStmt*>(case_stmt.get())->value.get()) {
            if (case_value->type && !stmt.expression->type->equals(*case_value->type)) {
                error(Token(TokenType::ERROR, "", nullptr, 0), "Case type must match switch expression type.");
            }
        }
    }
    switch_depth--;

    return nullptr;
}

std::any Resolver::visitCaseStmt(const CaseStmt& stmt) {
    if (stmt.value) {
        resolve(*stmt.value);
    }
    resolve(*stmt.body);
    return nullptr;
}

std::any Resolver::visitLambdaExpr(const LambdaExpr& expr) {
    CurrentFunctionType enclosingFunction = currentFunction;
    currentFunction = CurrentFunctionType::FUNCTION;

    auto enclosing_return_type = std::move(current_return_type);
    current_return_type = expr.return_type ? resolveTypeExpr(*expr.return_type) : std::make_shared<BasicType>("void");

    symbols.enter_scope();
    std::vector<std::shared_ptr<Type>> param_types;
    for (size_t i = 0; i < expr.params.size(); ++i) {
        auto param_type = resolveTypeExpr(*expr.param_types[i]);
        symbols.define(expr.params[i].lexeme, param_type);
        param_types.push_back(param_type);
    }

    resolve(*expr.body);

    symbols.exit_scope();

    currentFunction = enclosingFunction;
    current_return_type = std::move(enclosing_return_type);

    expr.type = std::make_shared<FunctionType>(current_return_type, param_types);
    return nullptr;
}

std::any Resolver::visitBreakStmt(const BreakStmt& stmt) {
    if (currentLoop == LoopType::NONE && switch_depth == 0) {
        error(stmt.keyword, "Cannot use 'break' outside of a loop or switch statement.");
    }
    return nullptr;
}

std::any Resolver::visitFallthroughStmt(const FallthroughStmt& stmt) {
    if (switch_depth == 0) {
        error(stmt.keyword, "Cannot use 'fallthrough' outside of a switch statement.");
    }
    return nullptr;
}

} // namespace chtholly
