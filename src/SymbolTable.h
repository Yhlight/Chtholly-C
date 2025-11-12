#ifndef CHTHOLLY_SYMBOLTABLE_H
#define CHTHOLLY_SYMBOLTABLE_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Token.h"

namespace chtholly {

// Forward declarations
class Type;

class SymbolTable {
public:
  SymbolTable();

  void enter_scope();
  void exit_scope();

  void define(const std::string& name, std::shared_ptr<Type> type);
  std::shared_ptr<Type> resolve(const Token& name);
  bool is_defined_in_current_scope(const std::string& name);

private:
  std::vector<std::map<std::string, std::shared_ptr<Type>>> scopes;
};

} // namespace chtholly

#endif // CHTHOLLY_SYMBOLTABLE_H
