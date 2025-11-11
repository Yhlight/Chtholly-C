#include "Resolver.h"
#include "AST.h"

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
    beginScope(); // Global scope
    // Pre-define all built-in functions and modules
    auto any_type = std::make_shared<BasicType>("any");
    auto void_type = std::make_shared<BasicType>("void");
    scopes.back()["print"] = std::make_shared<chtholly::FunctionType>(void_type, std::vector<std::shared_ptr<Type>>{any_type});
    scopes.back()["input"] = std::make_shared<chtholly::FunctionType>(std::make_shared<BasicType>("string"), std::vector<std::shared_ptr<Type>>{});
    scopes.back()["fs_read"] = std::make_shared<chtholly::FunctionType>(std::make_shared<BasicType>("string"), std::vector<std::shared_ptr<Type>>{std::make_shared<BasicType>("string")});
    scopes.back()["fs_write"] = any_type;
    scopes.back()["fs_exists"] = any_type;
    scopes.back()["fs_is_file"] = any_type;
    scopes.back()["fs_is_dir"] = any_type;
    scopes.back()["fs_list_dir"] = any_type;
    scopes.back()["fs_remove"] = any_type;
    scopes.back()["meta"] = any_type;
    scopes.back()["reflect"] = any_type;
    scopes.back()["util"] = any_type;
    scopes.back()["operator"] = any_type;
    scopes.back()["string"] = any_type;
    scopes.back()["array"] = any_type;
    scopes.back()["os"] = any_type;
    scopes.back()["time"] = any_type;
    scopes.back()["random"] = any_type;
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

void Resolver::beginScope() {
    scopes.push_back({});
}

void Resolver::endScope() {
    scopes.pop_back();
}

void Resolver::declare(const Token& name) {
    if (scopes.empty()) {
        return;
    }

    auto& scope = scopes.back();
    if (scope.count(name.lexeme)) {
        error(name, "Already a variable with this name in this scope.");
    }
    scope[name.lexeme] = nullptr; // Mark as declared but not defined
}

void Resolver::define(const Token& name, std::shared_ptr<Type> type) {
    if (scopes.empty()) {
        return;
    }

    scopes.back()[name.lexeme] = type;
}

std::any Resolver::visitBlockStmt(const BlockStmt& stmt) {
    beginScope();
    for (const auto& statement : stmt.statements) {
        resolve(*statement);
    }
    endScope();
    return nullptr;
}

std::any Resolver::visitVarStmt(const VarStmt& stmt) {
    declare(stmt.name);
    std::shared_ptr<Type> value_type = nullptr;
    if (stmt.initializer) {
        resolve(*stmt.initializer);
        value_type = stmt.initializer->type;
    }

    if (stmt.type) {
        auto declared_type = resolveTypeExpr(*stmt.type);
        if (value_type && declared_type && value_type->to_string() != declared_type->to_string()) {
            error(stmt.name, "Initializer type '" + value_type->to_string() +
                               "' does not match declared type '" + declared_type->to_string() + "'.");
        }
        value_type = declared_type;
    }

    define(stmt.name, value_type);
    return nullptr;
}

std::any Resolver::visitVariableExpr(const VariableExpr& expr) {
    if (!scopes.empty()) {
        auto& scope = scopes.back();
        auto it = scope.find(expr.name.lexeme);
        if (it != scope.end() && it->second == nullptr) {
            error(expr.name, "Can't read local variable in its own initializer.");
            return nullptr;
        }
    }

    for (int i = scopes.size() - 1; i >= 0; --i) {
        if (scopes[i].count(expr.name.lexeme)) {
            expr.type = scopes[i][expr.name.lexeme];
            return nullptr;
        }
    }

    error(expr.name, "Undefined variable.");
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
    error(Token(TokenType::AMPERSAND, "&", nullptr, 0), "Borrow types are not yet supported in type annotations.");
    return std::make_shared<BasicType>("unsupported");
}

std::any Resolver::visitAssignExpr(const AssignExpr& expr) {
    resolve(*expr.value);
    for (int i = scopes.size() - 1; i >= 0; --i) {
        if (scopes[i].count(expr.name.lexeme)) {
            return nullptr;
        }
    }
    error(expr.name, "Undefined variable.");
    return nullptr;
}

void Resolver::resolveFunction(const FunctionStmt& function, CurrentFunctionType type) {
    CurrentFunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (size_t i = 0; i < function.params.size(); ++i) {
        declare(function.params[i]);
        auto param_type = resolveTypeExpr(*function.param_types[i]);
        define(function.params[i], param_type);
    }
    resolve(*function.body);
    endScope();
    currentFunction = enclosingFunction;
}

std::any Resolver::visitFunctionStmt(const FunctionStmt& stmt) {
    declare(stmt.name);
    std::vector<std::shared_ptr<Type>> param_types;
    for (const auto& param_type_expr : stmt.param_types) {
        param_types.push_back(resolveTypeExpr(*param_type_expr));
    }
    auto return_type = stmt.return_type ? resolveTypeExpr(*stmt.return_type) : std::make_shared<BasicType>("void");
    auto func_type = std::make_shared<chtholly::FunctionType>(return_type, param_types);
    define(stmt.name, func_type);

    resolveFunction(stmt, CurrentFunctionType::FUNCTION);
    return nullptr;
}

std::any Resolver::visitExpressionStmt(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
    return nullptr;
}

std::any Resolver::visitIfStmt(const IfStmt& stmt) {
    resolve(*stmt.condition);
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
        resolve(*stmt.value);
    }
    return nullptr;
}

std::any Resolver::visitWhileStmt(const WhileStmt& stmt) {
    resolve(*stmt.condition);
    resolve(*stmt.body);
    return nullptr;
}

std::any Resolver::visitForStmt(const ForStmt& stmt) {
    if (stmt.initializer) {
        resolve(*stmt.initializer);
    }
    if (stmt.condition) {
        resolve(*stmt.condition);
    }
    if (stmt.increment) {
        resolve(*stmt.increment);
    }
    resolve(*stmt.body);
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

    auto it = op_to_trait.find(expr.op.type);
    if (it != op_to_trait.end()) {
        const std::string& trait_name = it->second;
        if (expr.left->type->get_kind() == TypeKind::STRUCT) {
            if (expr.right->type->get_kind() != TypeKind::STRUCT ||
                expr.left->type->to_string() != expr.right->type->to_string()) {
                error(expr.op, "Operands of operator " + expr.op.lexeme + " must be of the same struct type.");
                return nullptr;
            }

            auto struct_type = static_cast<const StructType*>(expr.left->type.get());
            if (!is_trait_implemented(struct_type->get_name(), trait_name)) {
                error(expr.op, "Struct '" + struct_type->get_name() + "' does not implement trait '" + trait_name + "' for this operator.");
            } else {
                expr.type = expr.left->type; // Result type is the same as operands
            }
        } else if (expr.left->type->get_kind() == TypeKind::BASIC && expr.right->type->get_kind() == TypeKind::BASIC) {
            // Basic type promotion logic (simplified)
            if (expr.left->type->to_string() == "double" || expr.right->type->to_string() == "double") {
                expr.type = std::make_shared<BasicType>("double");
            } else {
                expr.type = std::make_shared<BasicType>("int");
            }
        } else {
            error(expr.op, "Operator " + expr.op.lexeme + " cannot be applied to types '" + expr.left->type->to_string() + "' and '" + expr.right->type->to_string() + "'.");
        }
    } else {
        // Handle non-overloadable operators
        if (expr.left->type->get_kind() == TypeKind::BASIC && expr.right->type->get_kind() == TypeKind::BASIC) {
            expr.type = expr.left->type; // Simplistic result type
        }
    }

    return nullptr;
}

std::any Resolver::visitCallExpr(const CallExpr& expr) {
    resolve(*expr.callee);
    for (const auto& arg : expr.arguments) {
        resolve(*arg);
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
                    if (param_type && expr.arguments[i]->type->to_string() != param_type->to_string()) {
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
            if (expr.right->type->to_string() != "bool") {
                error(expr.op, "Operand of '!' must be of type 'bool'.");
            }
            expr.type = std::make_shared<BasicType>("bool");
            break;
        case TokenType::MINUS:
            if (expr.right->type->to_string() != "int" && expr.right->type->to_string() != "double") {
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
    expr.object->accept(*this);
    return nullptr;
}

std::any Resolver::visitSetExpr(const SetExpr& expr) {
    expr.value->accept(*this);
    expr.object->accept(*this);
    return nullptr;
}

std::any Resolver::visitSelfExpr(const SelfExpr& expr) {
    if (currentClass == ClassType::NONE) {
        error(expr.keyword, "Can't use 'self' outside of a class.");
        return nullptr;
    }

    for (int i = scopes.size() - 1; i >= 0; --i) {
        if (scopes[i].count("self")) {
            expr.type = scopes[i]["self"];
            return nullptr;
        }
    }

    return nullptr;
}

std::any Resolver::visitStructStmt(const StructStmt& stmt) {
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;

    declare(stmt.name);
    define(stmt.name, std::make_shared<StructType>(stmt.name.lexeme));

    beginScope();
    scopes.back()["self"] = std::make_shared<StructType>(stmt.name.lexeme);

    for (const auto& method : stmt.methods) {
        CurrentFunctionType declaration = CurrentFunctionType::METHOD;
        resolveFunction(*method, declaration);
    }

    endScope();

    currentClass = enclosingClass;
    return nullptr;
}

std::any Resolver::visitStructLiteralExpr(const StructLiteralExpr& expr) {
    auto it = structs.find(expr.name.lexeme);
    if (it != structs.end()) {
        expr.type = std::make_shared<StructType>(expr.name.lexeme);
    } else {
        error(expr.name, "Undefined struct '" + expr.name.lexeme + "'.");
    }
    return nullptr;
}

} // namespace chtholly
