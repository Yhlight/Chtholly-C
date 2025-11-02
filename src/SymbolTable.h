#ifndef CHTHOLLY_SYMBOLTABLE_H
#define CHTHOLLY_SYMBOLTABLE_H

#include "Symbol.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace chtholly {

class SymbolTable {
public:
    SymbolTable();

    void enterScope();
    void leaveScope();

    bool define(const std::string& name, std::shared_ptr<Symbol> symbol);
    Symbol* lookup(const std::string& name);

private:
    std::vector<std::unordered_map<std::string, std::shared_ptr<Symbol>>> scopes;
};

} // namespace chtholly

#endif // CHTHOLLY_SYMBOLTABLE_H