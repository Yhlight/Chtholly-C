#include "SymbolTable.h"

Symbol* SymbolTable::lookup(const std::string& name) {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto& scope = *it;
        if (scope.count(name)) {
            return &scope.at(name);
        }
    }
    return nullptr;
}

void SymbolTable::insert(const std::string& name, Symbol symbol) {
    if (!scopes_.empty()) {
        scopes_.back()[name] = symbol;
    }
}

void SymbolTable::enter_scope() {
    scopes_.emplace_back();
}

void SymbolTable::leave_scope() {
    scopes_.pop_back();
}
