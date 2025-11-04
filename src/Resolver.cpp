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
    }
    return {};
}

std::any Resolver::visitSwitchStmt(const SwitchStmt& stmt) {
    resolve(*stmt.expression);

    LoopType enclosingLoop = currentLoop;
    currentLoop = LoopType::SWITCH;

    for (const auto& case_stmt : stmt.cases) {
        if (case_stmt.condition) {
            resolve(*case_stmt.condition);
        }
        resolve(*case_stmt.body);
    }

    if (stmt.defaultCase) {
        resolve(*stmt.defaultCase->body);
    }

    currentLoop = enclosingLoop;
    return {};
}

std::any Resolver::visitBreakStmt(const BreakStmt& stmt) {
    if (currentLoop == LoopType::NONE) {
        ErrorReporter::error(stmt.keyword.line, "Can't use 'break' outside of a loop or switch statement.");
    }
    return {};
}

std::any Resolver::visitFallthroughStmt(const FallthroughStmt& stmt) {
    if (currentLoop != LoopType::SWITCH) {
        ErrorReporter::error(stmt.keyword.line, "Can't use 'fallthrough' outside of a switch statement.");
    }
    return {};
}

const std::vector<std::unique_ptr<Stmt>>& Resolver::get_statements() const {
    return statements;
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
                if (traitMethod->params[j].second.baseType.lexeme != implMethod->params[j].second.baseType.lexeme) {
                    ErrorReporter::error(implMethod->name.line, "Parameter type does not match trait.");
                }
            }
        }
    }

    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;
    beginScope();
    scopes.back()["self"] = VariableState{true, false, 0, false};
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
    chtholly.run(buffer.str());

    if (ErrorReporter::hadError) {
        return {};
    }

    return {};
}

std::any Resolver::visitGetExpr(const GetExpr& expr) {
    resolve(*expr.object);
    return {};
}

std::any Resolver::visitSetExpr(const SetExpr& expr) {
    resolve(*expr.value);
    resolve(*expr.object);
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
    LoopType enclosingLoop = currentLoop;
    currentLoop = LoopType::LOOP;
    resolve(*stmt.condition);
    resolve(*stmt.body);
    currentLoop = enclosingLoop;
    return {};
}

std::any Resolver::visitGroupingExpr(const GroupingExpr& expr) {
    resolve(*expr.expression);
    return {};
}

std::any Resolver::visitLiteralExpr(const LiteralExpr& expr) {
    return {};
}

std::any Resolver::visitUnaryExpr(const UnaryExpr& expr) {
    resolve(*expr.right);
    return {};
}
