#include "SymbolTable.h"

namespace chtholly {

SymbolTable::SymbolTable() {
    enterScope(); // Global scope
}

void SymbolTable::enterScope() {
    scopes.emplace_back();
}

void SymbolTable::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

bool SymbolTable::insert(const std::string& name, std::shared_ptr<Type> type, bool isMutable) {
    if (scopes.empty()) return false;
    auto& currentScope = scopes.back();
    if (currentScope.count(name)) {
        return false; // Symbol already exists in the current scope
    }
    currentScope[name] = {type, isMutable};
    return true;
}

std::optional<Symbol> SymbolTable::lookup(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto& scope = *it;
        if (scope.count(name)) {
            return scope[name];
        }
    }
    return std::nullopt;
}

} // namespace chtholly
