#include "Resolver.h"
#include "Error.h"

Resolver::Resolver() {
    scopes.emplace_back();
    scopes.back()["print"] = VariableState{true, false, 0, false};
    scopes.back()["input"] = VariableState{true, false, 0, false};
    scopes.back()["fs_read"] = VariableState{true, false, 0, false};
    scopes.back()["fs_write"] = VariableState{true, false, 0, false};
}

void Resolver::resolve(const std::vector<std::unique_ptr<Stmt>>& statements) {
    for (const auto& statement : statements) {
        if (auto* trait = dynamic_cast<TraitStmt*>(statement.get())) {
            traits[trait->name.lexeme] = trait;
        } else if (auto* s = dynamic_cast<StructStmt*>(statement.get())) {
            structs[s->name.lexeme] = s;
        }
    }

    for (const auto& statement : statements) {
        resolve(*statement);
    }
}

const std::map<std::string, VariableState>& Resolver::get_resolved_symbols() const {
    return scopes.front();
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
    if (scopes.empty()) return;
    if (scopes.back().count(name.lexeme)) {
        ErrorReporter::error(name.line, "Already a variable with this name in this scope.");
    }
    scopes.back()[name.lexeme] = VariableState();
}

void Resolver::define(const Token& name) {
    if (scopes.empty()) return;
    scopes.back()[name.lexeme].defined = true;
}

std::any Resolver::visitBlockStmt(const BlockStmt& stmt) {
    beginScope();
    resolve(stmt.statements);
    endScope();
    return {};
}

std::any Resolver::visitLetStmt(const LetStmt& stmt) {
    declare(stmt.name);
    if (stmt.initializer) {
        resolve(*stmt.initializer);
    }
    define(stmt.name);
    if (!scopes.empty()) {
        scopes.back()[stmt.name.lexeme].is_mutable = stmt.isMutable;
        if (stmt.type) {
            scopes.back()[stmt.name.lexeme].type = *stmt.type;
        } else if (stmt.initializer) {
            scopes.back()[stmt.name.lexeme].type = stmt.initializer->resolved_type;
        }
    }
    return {};
}

std::any Resolver::visitVariableExpr(const VariableExpr& expr) {
    if (!scopes.empty() && scopes.back().count(expr.name.lexeme) && !scopes.back().at(expr.name.lexeme).defined) {
        ErrorReporter::error(expr.name.line, "Can't read local variable in its own initializer.");
    }

    resolveLocal(expr, expr.name);
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i].count(expr.name.lexeme)) {
            if (scopes[i].at(expr.name.lexeme).moved) {
                ErrorReporter::error(expr.name.line, "Use of moved value.");
            }
            expr.resolved_type = scopes[i].at(expr.name.lexeme).type;
            return {};
        }
    }
    return {};
}

std::any Resolver::visitAssignExpr(const AssignExpr& expr) {
    resolve(*expr.value);
    resolveLocal(expr, expr.name);
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i].count(expr.name.lexeme)) {
            if (!scopes[i].at(expr.name.lexeme).is_mutable) {
                ErrorReporter::error(expr.name.line, "Cannot assign to an immutable variable.");
            }
            if (scopes[i].at(expr.name.lexeme).immutable_borrows > 0) {
                ErrorReporter::error(expr.name.line, "Cannot assign to a variable that is currently borrowed.");
            }
            expr.resolved_type = expr.value->resolved_type;
            return {};
        }
    }
    return {};
}

void Resolver::resolveLocal(const Expr& expr, const Token& name) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i].count(name.lexeme)) {
            return;
        }
    }

    ErrorReporter::error(name.line, "Use of unresolved variable '" + name.lexeme + "'.");
}

std::any Resolver::visitExpressionStmt(const ExpressionStmt& stmt) {
    resolve(*stmt.expression);
    return {};
}

std::any Resolver::visitBinaryExpr(const BinaryExpr& expr) {
    resolve(*expr.left);
    resolve(*expr.right);
    expr.resolved_type = expr.left->resolved_type;
    return {};
}

std::any Resolver::visitTraitStmt(const TraitStmt& stmt) {
    declare(stmt.name);
    define(stmt.name);
    beginScope();
    for (const auto& method : stmt.methods) {
        resolveFunction(*method, FunctionType::FUNCTION);
    }
    endScope();
    return {};
}

std::any Resolver::visitImplStmt(const ImplStmt& stmt) {
    if (stmt.traitName) {
        if (stmt.traitName->lexeme.rfind("operator::", 0) == 0) {
            if (!structs.count(stmt.structName.lexeme)) {
                ErrorReporter::error(stmt.structName.line, "Struct '" + stmt.structName.lexeme + "' not found.");
                return {};
            }
            std::string op = stmt.traitName->lexeme.substr(10);
            bool found = false;
            for(const auto& method : stmt.methods) {
                if (method->name.lexeme == op) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                ErrorReporter::error(stmt.structName.line, "Impl for operator '" + op + "' must contain a method named '" + op + "'.");
            }

        } else {
            if (traits.find(stmt.traitName->lexeme) == traits.end()) {
                ErrorReporter::error(stmt.traitName->line, "Trait '" + stmt.traitName->lexeme + "' not found.");
                return {};
            }
            if (!structs.count(stmt.structName.lexeme)) {
                ErrorReporter::error(stmt.structName.line, "Struct '" + stmt.structName.lexeme + "' not found.");
                return {};
            }

            const auto* trait = traits.at(stmt.traitName->lexeme);
            if (trait->methods.size() != stmt.methods.size()) {
                ErrorReporter::error(stmt.structName.line, "Impl does not implement all methods of the trait.");
                return {};
            }

            std::map<std::string, TypeInfo> generic_map;
            if (trait->generics.size() == stmt.generics.size()) {
                for (size_t i = 0; i < trait->generics.size(); ++i) {
                    generic_map[trait->generics[i].lexeme] = stmt.generics[i];
                }
            } else {
                ErrorReporter::error(stmt.structName.line, "Number of generic arguments does not match trait.");
                return {};
            }

            for (size_t i = 0; i < trait->methods.size(); ++i) {
                const auto& traitMethod = trait->methods[i];
                const auto& implMethod = stmt.methods[i];
                if (traitMethod->name.lexeme != implMethod->name.lexeme) {
                    ErrorReporter::error(implMethod->name.line, "Method name does not match trait.");
                }
                if (traitMethod->params.size() != implMethod->params.size()) {
                    ErrorReporter::error(implMethod->name.line, "Number of parameters does not match trait.");
                }
                for (size_t j = 0; j < traitMethod->params.size(); ++j) {
                    auto trait_param_type = traitMethod->params[j].second;
                    auto impl_param_type = implMethod->params[j].second;
                    if (generic_map.count(trait_param_type.baseType.lexeme)) {
                        trait_param_type = generic_map.at(trait_param_type.baseType.lexeme);
                    }
                    if (trait_param_type.baseType.lexeme != impl_param_type.baseType.lexeme) {
                        ErrorReporter::error(implMethod->name.line, "Parameter type does not match trait.");
                    }
                }
            }
        }
    }

    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;
    beginScope();
    scopes.back()["self"] = VariableState{true, true, 0, false, TypeInfo{stmt.structName}};
    for (const auto& method : stmt.methods) {
        resolveFunction(*method, FunctionType::FUNCTION);
    }
    endScope();
    currentClass = enclosingClass;
    return {};
}

#include <fstream>
#include <sstream>
#include "Chtholly.h"

std::any Resolver::visitImportStmt(const ImportStmt& stmt) {
    if (stmt.is_std) {
        if (stmt.path.lexeme != "iostream" && stmt.path.lexeme != "filesystem") {
            ErrorReporter::error(stmt.path.line, "Unknown standard library module.");
        }
        return {};
    }
    std::string path = std::get<std::string>(stmt.path.literal);
    std::ifstream file(path);
    if (!file.is_open()) {
        ErrorReporter::error(stmt.path.line, "Could not open file: " + path);
        return {};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    Chtholly chtholly;
    auto statements = chtholly.run(buffer.str(), true);

    if (ErrorReporter::hadError) {
        return {};
    }

    for (const auto& statement : statements) {
        if (auto* func = dynamic_cast<FunctionStmt*>(statement.get())) {
            declare(func->name);
            define(func->name);
        } else if (auto* s = dynamic_cast<StructStmt*>(statement.get())) {
            structs[s->name.lexeme] = s;
        } else if (auto* trait = dynamic_cast<TraitStmt*>(statement.get())) {
            traits[trait->name.lexeme] = trait;
        }
    }

    return {};
}

std::any Resolver::visitGetExpr(const GetExpr& expr) {
    resolve(*expr.object);
    if (auto* var = dynamic_cast<const VariableExpr*>(expr.object.get())) {
        if (var->name.lexeme == "self") {
            if (currentClass == ClassType::NONE) {
                ErrorReporter::error(var->name.line, "Can't use 'self' outside of a class.");
                return {};
            }
        }
    }

    if (expr.object->resolved_type) {
        if (structs.count(expr.object->resolved_type->baseType.lexeme)) {
            auto s = structs.at(expr.object->resolved_type->baseType.lexeme);
            for (const auto& field : s->fields) {
                if (field->name.lexeme == expr.name.lexeme) {
                    expr.resolved_type = field->type;
                }
            }
        }
    }
    return {};
}

std::any Resolver::visitSetExpr(const SetExpr& expr) {
    resolve(*expr.value);
    resolve(*expr.object);
    expr.resolved_type = expr.value->resolved_type;
    return {};
}

std::any Resolver::visitBorrowExpr(const BorrowExpr& expr) {
    resolve(*expr.expression);
    if (auto* var = dynamic_cast<const VariableExpr*>(expr.expression.get())) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].count(var->name.lexeme)) {
                if (expr.isMutable) {
                    if (!scopes[i].at(var->name.lexeme).is_mutable) {
                        ErrorReporter::error(var->name.line, "Cannot mutably borrow an immutable variable.");
                    }
                    if (scopes[i].at(var->name.lexeme).immutable_borrows > 0) {
                        ErrorReporter::error(var->name.line, "Cannot mutably borrow a variable that is currently immutably borrowed.");
                    }
                    scopes[i][var->name.lexeme].moved = true;
                } else {
                    scopes[i][var->name.lexeme].immutable_borrows++;
                }
                expr.resolved_type = expr.expression->resolved_type;
                if (expr.resolved_type) {
                    expr.resolved_type->isReference = true;
                    expr.resolved_type->isMutable = expr.isMutable;
                }
                return {};
            }
        }
    }
    return {};
}

std::any Resolver::visitLambdaExpr(const LambdaExpr& expr) {
    resolveLambda(expr, FunctionType::FUNCTION);
    return {};
}

std::any Resolver::visitStructStmt(const StructStmt& stmt) {
    declare(stmt.name);
    define(stmt.name);
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;

    beginScope();
    scopes.back()["self"] = VariableState{true, false, 0, false};

    for (const auto& field : stmt.fields) {
        resolve(*field);
    }
    endScope();
    currentClass = enclosingClass;
    return {};
}

std::any Resolver::visitCallExpr(const CallExpr& expr) {
    resolve(*expr.callee);

    for (const auto& argument : expr.arguments) {
        resolve(*argument);
    }

    if (expr.callee->resolved_type && expr.callee->resolved_type->returnType) {
        expr.resolved_type = *expr.callee->resolved_type->returnType;
    }

    return {};
}

void Resolver::resolveFunction(const FunctionStmt& function, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (const auto& generic : function.generics) {
        declare(generic);
        define(generic);
    }
    for (const auto& param : function.params) {
        declare(param.first);
        define(param.first);
    }
    resolve(function.body);
    endScope();
    currentFunction = enclosingFunction;
}

void Resolver::resolveLambda(const LambdaExpr& lambda, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (const auto& param : lambda.params) {
        declare(param.first);
        define(param.first);
    }
    resolve(lambda.body);
    endScope();
    currentFunction = enclosingFunction;
}

std::any Resolver::visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName) {
    declare(stmt.name);
    define(stmt.name);
    resolveFunction(stmt, FunctionType::FUNCTION);
    return {};
}

std::any Resolver::visitReturnStmt(const ReturnStmt& stmt) {
    if (currentFunction == FunctionType::NONE) {
        ErrorReporter::error(stmt.keyword.line, "Can't return from top-level code.");
    }

    if (stmt.value) {
        resolve(*stmt.value);
    }

    return {};
}

std::any Resolver::visitIfStmt(const IfStmt& stmt) {
    resolve(*stmt.condition);
    resolve(*stmt.thenBranch);
    if (stmt.elseBranch) resolve(*stmt.elseBranch);
    return {};
}

std::any Resolver::visitWhileStmt(const WhileStmt& stmt) {
    resolve(*stmt.condition);
    resolve(*stmt.body);
    return {};
}

std::any Resolver::visitGroupingExpr(const GroupingExpr& expr) {
    resolve(*expr.expression);
    expr.resolved_type = expr.expression->resolved_type;
    return {};
}

std::any Resolver::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<double>(expr.value)) {
        expr.resolved_type = TypeInfo{Token{TokenType::IDENTIFIER, "double", std::monostate{}, -1}};
    } else if (std::holds_alternative<bool>(expr.value)) {
        expr.resolved_type = TypeInfo{Token{TokenType::IDENTIFIER, "bool", std::monostate{}, -1}};
    } else if (std::holds_alternative<std::string>(expr.value)) {
        expr.resolved_type = TypeInfo{Token{TokenType::IDENTIFIER, "string", std::monostate{}, -1}};
    }
    return {};
}

std::any Resolver::visitUnaryExpr(const UnaryExpr& expr) {
    resolve(*expr.right);
    expr.resolved_type = expr.right->resolved_type;
    return {};
}

std::any Resolver::visitStructInitializerExpr(const StructInitializerExpr& expr) {
    if (structs.find(expr.name.lexeme) == structs.end()) {
        ErrorReporter::error(expr.name.line, "Struct '" + expr.name.lexeme + "' not found.");
        return {};
    }

    const auto* struct_decl = structs.at(expr.name.lexeme);
    if (struct_decl->fields.size() != expr.initializers.size()) {
        ErrorReporter::error(expr.name.line, "Incorrect number of fields in initializer for struct '" + expr.name.lexeme + "'.");
    }

    for (const auto& [name, value] : expr.initializers) {
        bool found = false;
        for (const auto& field : struct_decl->fields) {
            if (field->name.lexeme == name.lexeme) {
                found = true;
                resolve(*value);
                if (field->type && value->resolved_type && field->type->baseType.lexeme != value->resolved_type->baseType.lexeme) {
                    ErrorReporter::error(name.line, "Type mismatch for field '" + name.lexeme + "'.");
                }
                break;
            }
        }
        if (!found) {
            ErrorReporter::error(name.line, "Struct '" + expr.name.lexeme + "' does not have a field named '" + name.lexeme + "'.");
        }
    }

    expr.resolved_type = TypeInfo{expr.name};
    return {};
}
