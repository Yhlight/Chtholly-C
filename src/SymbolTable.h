#ifndef CHTHOLLY_SYMBOL_TABLE_H
#define CHTHOLLY_SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Type.h"

struct Symbol {
    bool isDefined;
    bool isMutable;
    std::unique_ptr<Type> type;
};

class SymbolTable {
public:
    SymbolTable();

    void beginScope();
    void endScope();

    void declare(const std::string& name, bool isMutable, std::unique_ptr<Type> type);
    void define(const std::string& name);

    bool isDeclared(const std::string& name);
    bool isDeclaredInCurrentScope(const std::string& name);
    bool isMutable(const std::string& name);

private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes_;
};

#endif // CHTHOLLY_SYMBOL_TABLE_H
