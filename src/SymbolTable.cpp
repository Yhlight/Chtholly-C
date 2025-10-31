#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    beginScope(); // Global scope
}

void SymbolTable::beginScope() {
    scopes_.emplace_back();
}

void SymbolTable::endScope() {
    scopes_.pop_back();
}

void SymbolTable::declare(const std::string& name) {
    if (scopes_.empty()) return;
    scopes_.back()[name] = false;
}

void SymbolTable::define(const std::string& name) {
    if (scopes_.empty()) return;
    scopes_.back()[name] = true;
}

bool SymbolTable::isDeclared(const std::string& name) {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        if (it->count(name)) {
            return true;
        }
    }
    return false;
}

bool SymbolTable::isDeclaredInCurrentScope(const std::string& name) {
    if (scopes_.empty()) return false;
    return scopes_.back().count(name);
}
