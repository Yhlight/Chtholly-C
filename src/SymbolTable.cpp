#include "SymbolTable.h"

namespace chtholly {

SymbolTable::SymbolTable() {
    // Start with a global scope
    enterScope();
}

void SymbolTable::enterScope() {
    scopes.emplace_back();
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

bool SymbolTable::define(const std::string& name, std::unique_ptr<Type> type, Mutability mutability) {
    if (scopes.empty()) {
        return false; // Should always have at least the global scope
    }

    auto& currentScope = scopes.back();
    if (currentScope.find(name) != currentScope.end()) {
        return false; // Symbol already defined in the current scope
    }

    currentScope.emplace(name, Symbol(name, std::move(type), mutability));
    return true;
}

Symbol* SymbolTable::lookup(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto& scope = *it;
        auto symbolIt = scope.find(name);
        if (symbolIt != scope.end()) {
            return &symbolIt->second;
        }
    }
    return nullptr; // Symbol not found
}

} // namespace chtholly
