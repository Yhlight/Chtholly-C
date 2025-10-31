#include "SymbolTable.h"
#include "Type.h"

SymbolTable::SymbolTable() {
    beginScope(); // Global scope
}

void SymbolTable::beginScope() {
    scopes_.emplace_back();
}

void SymbolTable::endScope() {
    scopes_.pop_back();
}

void SymbolTable::declare(const std::string& name, bool isMutable, std::unique_ptr<Type> type) {
    if (scopes_.empty()) return;
    scopes_.back()[name] = {false, isMutable, std::move(type)};
}

void SymbolTable::define(const std::string& name) {
    if (scopes_.empty()) return;
    scopes_.back().at(name).isDefined = true;
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

bool SymbolTable::isMutable(const std::string& name) {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        if (it->count(name)) {
            return it->at(name).isMutable;
        }
    }
    return false; // Should not happen if isDeclared is called first
}
