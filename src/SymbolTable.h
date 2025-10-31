#pragma once

#include "Symbol.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace chtholly {

class SymbolTable {
public:
    SymbolTable();

    void enterScope();
    void exitScope();

    bool define(const std::string& name, std::unique_ptr<Type> type, Mutability mutability);

    Symbol* lookup(const std::string& name);

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
};

} // namespace chtholly
