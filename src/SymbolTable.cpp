#include "SymbolTable.h"

namespace chtholly {

SymbolTable::SymbolTable() {
    enterScope(); // Global scope
}

void SymbolTable::enterScope() {
    scopes.emplace_back();
}

void SymbolTable::leaveScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

bool SymbolTable::define(const std::string& name, std::shared_ptr<Symbol> symbol) {
    if (scopes.empty()) {
        return false;
    }
    return scopes.back().insert({name, symbol}).second;
}

Symbol* SymbolTable::lookup(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second.get();
        }
    }
    return nullptr;
}

} // namespace chtholly