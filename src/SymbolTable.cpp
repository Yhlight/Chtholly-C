#include "SymbolTable.h"
#include "Type.h"
#include "Token.h"

namespace chtholly {

SymbolTable::SymbolTable() {
  // Start with a single global scope.
  enter_scope();
}

void SymbolTable::enter_scope() {
  scopes.emplace_back();
}

void SymbolTable::exit_scope() {
  if (!scopes.empty()) {
    scopes.pop_back();
  }
}

void SymbolTable::define(const std::string& name, std::shared_ptr<Type> type) {
  if (scopes.empty()) {
    // Should not happen, constructor ensures at least one scope.
    return;
  }
  scopes.back()[name] = type;
}

std::shared_ptr<Type> SymbolTable::resolve(const Token& name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name.lexeme);
    if (found != it->end()) {
      return found->second;
    }
  }
  return nullptr; // Not found
}

bool SymbolTable::is_defined_in_current_scope(const std::string& name) {
    if (scopes.empty()) {
        return false;
    }
    const auto& current_scope = scopes.back();
    return current_scope.find(name) != current_scope.end();
}

} // namespace chtholly
