#ifndef CHTHOLLY_SYMBOL_TABLE_H
#define CHTHOLLY_SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <unordered_map>

class SymbolTable {
public:
    SymbolTable();

    void beginScope();
    void endScope();

    void declare(const std::string& name);
    void define(const std::string& name);

    bool isDeclared(const std::string& name);
    bool isDeclaredInCurrentScope(const std::string& name);

private:
    std::vector<std::unordered_map<std::string, bool>> scopes_;
};

#endif // CHTHOLLY_SYMBOL_TABLE_H
