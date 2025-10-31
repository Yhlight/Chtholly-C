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

const Symbol* SymbolTable::lookup(const std::string& name) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        const auto& scope = *it;
        auto symbolIt = scope.find(name);
        if (symbolIt != scope.end()) {
            return &symbolIt->second;
        }
    }
    return nullptr;
}

} // namespace chtholly
