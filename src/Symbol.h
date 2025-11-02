#ifndef CHTHOLLY_SYMBOL_H
#define CHTHOLLY_SYMBOL_H

#include "Type.h"
#include "Token.h"
#include <string>
#include <memory>

namespace chtholly {

struct Symbol {
    std::string name;
    std::shared_ptr<Type> type;
    bool isMutable;
    // Add other properties as needed, e.g., scope level, definition location
};

} // namespace chtholly

#endif // CHTHOLLY_SYMBOL_H