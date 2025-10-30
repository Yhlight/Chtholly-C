#pragma once

#include "Type.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <optional>

namespace chtholly {

struct Symbol {
    std::shared_ptr<Type> type;
    bool isMutable;
};

class SymbolTable {
public:
    SymbolTable();

    void enterScope();
    void exitScope();

    bool insert(const std::string& name, std::shared_ptr<Type> type, bool isMutable);
    std::optional<Symbol> lookup(const std::string& name);

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
};

} // namespace chtholly
