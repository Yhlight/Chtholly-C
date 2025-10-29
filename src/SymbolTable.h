#ifndef CHTHOLLY_SYMBOLTABLE_H
#define CHTHOLLY_SYMBOLTABLE_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Chtholly {

struct Symbol {
    std::string name;
    Token type;
};

class SymbolTable {
public:
    SymbolTable();

    void enterScope();
    void exitScope();

    bool define(const std::string& name, const Token& type);
    Symbol* lookup(const std::string& name);

private:
    using Scope = std::unordered_map<std::string, Symbol>;
    std::vector<Scope> scopes;
};

} // namespace Chtholly

#endif // CHTHOLLY_SYMBOLTABLE_H
