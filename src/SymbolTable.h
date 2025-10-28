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

#include "Type.h"

struct Symbol {
    std::string name;
    OwnershipState state;
    std::shared_ptr<Type> type;
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
