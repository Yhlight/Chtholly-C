#include "SymbolTable.h"

namespace Chtholly {

SymbolTable::SymbolTable() {
    enterScope(); // Global scope
}

void SymbolTable::enterScope() {
    scopes.emplace_back();
}

void SymbolTable::exitScope() {
    scopes.pop_back();
}

bool SymbolTable::define(const std::string& name, const Token& type) {
    if (scopes.empty()) {
        return false; // Should not happen
    }
    if (scopes.back().count(name)) {
        return false; // Already defined in this scope
    }
    scopes.back()[name] = {name, type};
    return true;
}

Symbol* SymbolTable::lookup(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto symbolIt = it->find(name);
        if (symbolIt != it->end()) {
            return &symbolIt->second;
        }
    }
    return nullptr;
}

} // namespace Chtholly
