#pragma once

#include "Type.h"
#include <string>
#include <memory>

namespace chtholly {

enum class Mutability {
    Immutable, // let
    Mutable    // mut
};

struct Symbol {
    std::string name;
    std::unique_ptr<Type> type;
    Mutability mutability;

    Symbol(std::string name, std::unique_ptr<Type> type, Mutability mutability)
        : name(std::move(name)), type(std::move(type)), mutability(mutability) {}
};

} // namespace chtholly
