#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>

enum class OwnershipState {
    Owned,
    Moved,
    Borrowed
};

struct Symbol {
    std::string name;
    OwnershipState state;
    // We will add type information here later
};

class SymbolTable {
public:
    Symbol* lookup(const std::string& name);
    void insert(const std::string& name, Symbol symbol);
    void enter_scope();
    void leave_scope();

private:
    std::vector<std::map<std::string, Symbol>> scopes_;
};
