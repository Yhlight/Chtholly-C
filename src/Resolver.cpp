#include "Resolver.h"
#include "TypeSystem.h"

namespace chtholly {

class Resolver::TypeResolver {
public:
    std::shared_ptr<types::Type> resolve(const TypeExpr& type) {
        if (auto baseType = dynamic_cast<const BaseTypeExpr*>(&type)) {
            return std::make_shared<types::PrimitiveType>(toPrimitiveKind(baseType->type.lexeme));
        }
        // TODO: Handle other types like array, function, etc.
        return std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO);
    }
private:
    types::PrimitiveType::Kind toPrimitiveKind(const std::string& lexeme) {
        if (lexeme == "int") return types::PrimitiveType::Kind::INT;
        if (lexeme == "uint") return types::PrimitiveType::Kind::UINT;
        if (lexeme == "double") return types::PrimitiveType::Kind::DOUBLE;
        if (lexeme == "bool") return types::PrimitiveType::Kind::BOOL;
        if (lexeme == "char") return types::PrimitiveType::Kind::CHAR;
        if (lexeme == "string") return types::PrimitiveType::Kind::STRING;
        if (lexeme == "void") return types::PrimitiveType::Kind::VOID;
        return types::PrimitiveType::Kind::AUTO;
    }
};


Resolver::Resolver() : typeResolver(std::make_unique<TypeResolver>()) {
    beginScope(); // Global scope

    // Pre-define all built-in functions and modules
    auto void_type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::VOID);
    auto string_type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::STRING);
    auto bool_type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::BOOL);
    auto any_type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO);

    // Built-in functions
    // print(any) -> void
    scopes.back()["print"] = std::make_shared<types::FunctionType>(void_type, std::vector<std::shared_ptr<types::Type>>{any_type});
    // input() -> string
    scopes.back()["input"] = std::make_shared<types::FunctionType>(string_type, std::vector<std::shared_ptr<types::Type>>{});
    // fs_read(string) -> string
    scopes.back()["fs_read"] = std::make_shared<types::FunctionType>(string_type, std::vector<std::shared_ptr<types::Type>>{string_type});
    // fs_write(string, string) -> void
    scopes.back()["fs_write"] = std::make_shared<types::FunctionType>(void_type, std::vector<std::shared_ptr<types::Type>>{string_type, string_type});
    // fs_exists(string) -> bool
    scopes.back()["fs_exists"] = std::make_shared<types::FunctionType>(bool_type, std::vector<std::shared_ptr<types::Type>>{string_type});
    // fs_is_file(string) -> bool
    scopes.back()["fs_is_file"] = std::make_shared<types::FunctionType>(bool_type, std::vector<std::shared_ptr<types::Type>>{string_type});
    // fs_is_dir(string) -> bool
    scopes.back()["fs_is_dir"] = std::make_shared<types::FunctionType>(bool_type, std::vector<std::shared_ptr<types::Type>>{string_type});
    // fs_list_dir(string) -> array<string>  (NOTE: Array type not implemented yet, using AUTO)
    scopes.back()["fs_list_dir"] = std::make_shared<types::FunctionType>(any_type, std::vector<std::shared_ptr<types::Type>>{string_type});
    // fs_remove(string) -> void
    scopes.back()["fs_remove"] = std::make_shared<types::FunctionType>(void_type, std::vector<std::shared_ptr<types::Type>>{string_type});

    // Modules (represented as AUTO for now)
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

Resolver::~Resolver() = default;

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
    for (const auto& statement : statements) {
        resolveStmt(*statement);
    }
}

void Resolver::resolveStmt(const Stmt& stmt) {
    stmt.accept(*this);
}

std::any Resolver::resolveExpr(const Expr& expr) {
    return expr.accept(*this);
}

void Resolver::beginScope() {
    scopes.push_back({});
    defined_scopes.push_back({});
}

void Resolver::endScope() {
    scopes.pop_back();
    defined_scopes.pop_back();
}

void Resolver::declare(const Token& name) {
    if (scopes.empty()) {
        return;
    }

    auto& scope = scopes.back();
    if (scope.count(name.lexeme)) {
        error(name, "Already a variable with this name in this scope.");
    }
    scope[name.lexeme] = nullptr; // Initially null
    defined_scopes.back()[name.lexeme] = false;
}

void Resolver::define(const Token& name, std::shared_ptr<types::Type> type) {
    if (scopes.empty()) {
        return;
    }
    scopes.back()[name.lexeme] = type;
    defined_scopes.back()[name.lexeme] = true;
}

std::shared_ptr<types::Type> Resolver::lookup(const Token& name) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i].count(name.lexeme)) {
            return scopes[i][name.lexeme];
        }
    }
    return nullptr;
}

std::any Resolver::visitBlockStmt(const BlockStmt& stmt) {
    beginScope();
    for(const auto& s : stmt.statements) {
        resolveStmt(*s);
    }
    endScope();
    return nullptr;
}

std::any Resolver::visitVarStmt(const VarStmt& stmt) {
    declare(stmt.name);
    std::shared_ptr<types::Type> varType = nullptr;

    if (stmt.initializer) {
        varType = std::any_cast<std::shared_ptr<types::Type>>(resolveExpr(*stmt.initializer));
    }

    if (stmt.type) {
        auto declaredType = typeResolver->resolve(*stmt.type);
        if (varType && declaredType->toString() != "auto" && varType->toString() != "auto" && !declaredType->isEqualTo(*varType)) {
            error(stmt.name, "Initializer type does not match variable's declared type.");
        }
        varType = declaredType;
    }

    if (!varType) {
        // This can happen if there's no type annotation and no initializer
        error(stmt.name, "Cannot infer type for variable with no initializer.");
        varType = std::shared_ptr<types::Type>(std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO));
    }

    define(stmt.name, varType);
    return nullptr;
}

std::any Resolver::visitVariableExpr(const VariableExpr& expr) {
    if (!scopes.empty()) {
        auto it = defined_scopes.back().find(expr.name.lexeme);
        if (it != defined_scopes.back().end() && !it->second) {
            error(expr.name, "Can't read local variable in its own initializer.");
        }
    }

    auto type = lookup(expr.name);
    if (type == nullptr) {
        error(expr.name, "Undefined variable.");
    }
    return type;
}

std::any Resolver::visitAssignExpr(const AssignExpr& expr) {
    auto varType = lookup(expr.name);
    if (varType == nullptr) {
        error(expr.name, "Undefined variable.");
        return nullptr;
    }

    auto valueType = std::any_cast<std::shared_ptr<types::Type>>(resolveExpr(*expr.value));

    if (varType->toString() != "auto" && valueType->toString() != "auto" && !varType->isEqualTo(*valueType)) {
        error(expr.name, "Type mismatch in assignment.");
    }

    return nullptr;
}

void Resolver::resolveFunction(const FunctionStmt& function, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (size_t i = 0; i < function.params.size(); ++i) {
        declare(function.params[i]);
        define(function.params[i], typeResolver->resolve(*function.param_types[i]));
    }
    for(const auto& s : function.body->statements) {
        resolveStmt(*s);
    }
    endScope();
    currentFunction = enclosingFunction;
}

std::any Resolver::visitFunctionStmt(const FunctionStmt& stmt) {
    declare(stmt.name);
    std::shared_ptr<types::Type> type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO);
    if (stmt.return_type) {
        type = typeResolver->resolve(*stmt.return_type);
    }
    define(stmt.name, type);

    resolveFunction(stmt, FunctionType::FUNCTION);
    return nullptr;
}

std::any Resolver::visitExpressionStmt(const ExpressionStmt& stmt) {
    resolveExpr(*stmt.expression);
    return nullptr;
}

std::any Resolver::visitIfStmt(const IfStmt& stmt) {
    resolveExpr(*stmt.condition);
    resolveStmt(*stmt.thenBranch);
    if (stmt.elseBranch) {
        resolveStmt(*stmt.elseBranch);
    }
    return nullptr;
}

std::any Resolver::visitReturnStmt(const ReturnStmt& stmt) {
    if (currentFunction == FunctionType::NONE) {
        error(stmt.keyword, "Can't return from top-level code.");
    }

    if (stmt.value) {
        resolveExpr(*stmt.value);
    }
    return nullptr;
}

std::any Resolver::visitWhileStmt(const WhileStmt& stmt) {
    resolveExpr(*stmt.condition);
    resolveStmt(*stmt.body);
    return nullptr;
}

std::any Resolver::visitForStmt(const ForStmt& stmt) {
    if (stmt.initializer) {
        resolveStmt(*stmt.initializer);
    }
    if (stmt.condition) {
        resolveExpr(*stmt.condition);
    }
    if (stmt.increment) {
        resolveExpr(*stmt.increment);
    }
    resolveStmt(*stmt.body);
    return nullptr;
}

std::any Resolver::visitBinaryExpr(const BinaryExpr& expr) {
    auto leftType = std::any_cast<std::shared_ptr<types::Type>>(resolveExpr(*expr.left));
    auto rightType = std::any_cast<std::shared_ptr<types::Type>>(resolveExpr(*expr.right));

    if (leftType && rightType) {
        if (auto pLeft = std::dynamic_pointer_cast<types::PrimitiveType>(leftType)) {
            if (auto pRight = std::dynamic_pointer_cast<types::PrimitiveType>(rightType)) {
                std::shared_ptr<types::Type> resultType;
                if (pLeft->kind == types::PrimitiveType::Kind::INT && pRight->kind == types::PrimitiveType::Kind::INT) {
                    resultType = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::INT);
                } else if ((pLeft->kind == types::PrimitiveType::Kind::DOUBLE || pLeft->kind == types::PrimitiveType::Kind::INT) &&
                           (pRight->kind == types::PrimitiveType::Kind::DOUBLE || pRight->kind == types::PrimitiveType::Kind::INT)) {
                    resultType = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::DOUBLE);
                } else {
                    error(expr.op, "Invalid operands for binary expression.");
                    resultType = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO);
                }
                return resultType;
            }
        }
    }
    return std::shared_ptr<types::Type>(std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO));
}

std::any Resolver::visitCallExpr(const CallExpr& expr) {
    auto calleeTypeAny = resolveExpr(*expr.callee);
    if (!calleeTypeAny.has_value()) {
        // Error already reported during callee resolution
        return std::shared_ptr<types::Type>(std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO));
    }
    auto calleeType = std::any_cast<std::shared_ptr<types::Type>>(calleeTypeAny);

    if (auto funcType = std::dynamic_pointer_cast<types::FunctionType>(calleeType)) {
        // Check arity
        if (expr.arguments.size() != funcType->param_types.size()) {
            // A special case for print, which can take any number of arguments
            auto variableExpr = dynamic_cast<const VariableExpr*>(&*expr.callee);
            if (!variableExpr || variableExpr->name.lexeme != "print") {
                error(expr.paren, "Expected " + std::to_string(funcType->param_types.size()) +
                                 " arguments but got " + std::to_string(expr.arguments.size()) + ".");
            }
        }

        // Check argument types
        for (size_t i = 0; i < expr.arguments.size(); ++i) {
            if (i < funcType->param_types.size()) {
                auto argType = std::any_cast<std::shared_ptr<types::Type>>(resolveExpr(*expr.arguments[i]));
                auto paramType = funcType->param_types[i];

                if (paramType->toString() != "auto" && argType->toString() != "auto" && !paramType->isEqualTo(*argType)) {
                     error(expr.paren, "Argument type mismatch for parameter " + std::to_string(i + 1) +
                                      ". Expected " + paramType->toString() + " but got " + argType->toString() + ".");
                }
            }
        }
        return funcType->return_type;

    } else if (std::dynamic_pointer_cast<types::PrimitiveType>(calleeType) &&
               calleeType->toString() == "auto") {
        // Callee is a module or something we haven't resolved a concrete type for.
        // We can't check it, so we just resolve the arguments and assume it's okay.
        for (const auto& arg : expr.arguments) {
            resolveExpr(*arg);
        }
    }
    else {
        error(expr.paren, "Can only call functions and classes.");
    }

    return std::shared_ptr<types::Type>(std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO));
}

std::any Resolver::visitGroupingExpr(const GroupingExpr& expr) {
    return resolveExpr(*expr.expression);
}

std::any Resolver::visitLiteralExpr(const LiteralExpr& expr) {
    std::shared_ptr<types::Type> type;
    if (std::holds_alternative<long long>(expr.value)) {
        type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::INT);
    } else if (std::holds_alternative<double>(expr.value)) {
        type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::DOUBLE);
    } else if (std::holds_alternative<std::string>(expr.value)) {
        type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::STRING);
    } else if (std::holds_alternative<bool>(expr.value)) {
        type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::BOOL);
    } else if (std::holds_alternative<char>(expr.value)) {
        type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::CHAR);
    } else {
        type = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::VOID);
    }
    return type;
}

std::any Resolver::visitUnaryExpr(const UnaryExpr& expr) {
    return resolveExpr(*expr.right);
}

std::any Resolver::visitGetExpr(const GetExpr& expr) {
    resolveExpr(*expr.object);
    return std::shared_ptr<types::Type>(std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO));
}

std::any Resolver::visitSetExpr(const SetExpr& expr) {
    resolveExpr(*expr.value);
    resolveExpr(*expr.object);
    return nullptr;
}

std::any Resolver::visitSelfExpr(const SelfExpr& expr) {
    if (currentClass == ClassType::NONE) {
        error(expr.keyword, "Can't use 'self' outside of a class.");
        return std::shared_ptr<types::Type>(std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO));
    }

    return lookup(expr.keyword);
}

std::any Resolver::visitStructStmt(const StructStmt& stmt) {
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;

    declare(stmt.name);
    define(stmt.name, std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO));

    beginScope();
    scopes.back()["self"] = std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO);

    for (const auto& method : stmt.methods) {
        FunctionType declaration = FunctionType::FUNCTION;
        resolveFunction(*method, declaration);
    }

    endScope();

    currentClass = enclosingClass;
    return nullptr;
}
std::any Resolver::visitSwitchStmt(const SwitchStmt& stmt) {
    resolveExpr(*stmt.expression);
    for (const auto& a_case : stmt.cases) {
        resolveStmt(*a_case);
    }
    return nullptr;
}
std::any Resolver::visitCaseStmt(const CaseStmt& stmt) {
    if (stmt.value) {
        resolveExpr(*stmt.value);
    }
    resolveStmt(*stmt.body);
    return nullptr;
}
std::any Resolver::visitBreakStmt(const BreakStmt& stmt) {
    // TODO: Add validation to ensure break is inside a loop or switch
    return nullptr;
}
std::any Resolver::visitFallthroughStmt(const FallthroughStmt& stmt) {
    // TODO: Add validation to ensure fallthrough is inside a switch
    return nullptr;
}
std::any Resolver::visitLambdaExpr(const LambdaExpr& expr) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = FunctionType::FUNCTION;

    beginScope();
    for (size_t i = 0; i < expr.params.size(); ++i) {
        declare(expr.params[i]);
        define(expr.params[i], typeResolver->resolve(*expr.param_types[i]));
    }
    for(const auto& s : expr.body->statements) {
        resolveStmt(*s);
    }
    endScope();
    currentFunction = enclosingFunction;
    return std::shared_ptr<types::Type>(std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO));
}
std::any Resolver::visitArrayLiteralExpr(const ArrayLiteralExpr& expr) {
    if (expr.elements.empty()) {
        return std::shared_ptr<types::Type>(std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO));
    }
    std::shared_ptr<types::Type> firstType = nullptr;
    for (const auto& element : expr.elements) {
        auto elementType = std::any_cast<std::shared_ptr<types::Type>>(resolveExpr(*element));
        if (!firstType) {
            firstType = elementType;
        } else {
            if (firstType->toString() != "auto" && elementType->toString() != "auto" && !firstType->isEqualTo(*elementType)) {
                // error(expr.elements[0]->start, "Array elements must have the same type.");
            }
        }
    }
    return firstType;
}

std::any Resolver::visitImportStmt(const ImportStmt& ) {
    return nullptr;
}

std::any Resolver::visitEnumStmt(const EnumStmt& ) {
    return nullptr;
}

std::any Resolver::visitTraitStmt(const TraitStmt& ) {
    return nullptr;
}

std::any Resolver::visitBorrowExpr(const BorrowExpr& expr) {
    return resolveExpr(*expr.expression);
}

std::any Resolver::visitDerefExpr(const DerefExpr& expr) {
    return resolveExpr(*expr.expression);
}

std::any Resolver::visitStructLiteralExpr(const StructLiteralExpr& ) {
    return std::shared_ptr<types::Type>(std::make_shared<types::PrimitiveType>(types::PrimitiveType::Kind::AUTO));
}

std::any Resolver::visitTypeCastExpr(const TypeCastExpr& expr) {
    resolveExpr(*expr.expression);
    return typeResolver->resolve(*expr.type);
}

} // namespace chtholly
