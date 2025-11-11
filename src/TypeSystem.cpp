#include "TypeSystem.h"

namespace chtholly {
namespace types {

std::string PrimitiveType::toString() const {
    switch (kind) {
        case Kind::INT: return "int";
        case Kind::UINT: return "uint";
        case Kind::DOUBLE: return "double";
        case Kind::BOOL: return "bool";
        case Kind::CHAR: return "char";
        case Kind::STRING: return "string";
        case Kind::VOID: return "void";
        case Kind::AUTO: return "auto";
    }
    return "unknown";
}

bool PrimitiveType::isEqualTo(const Type& other) const {
    if (auto otherPrimitive = dynamic_cast<const PrimitiveType*>(&other)) {
        return kind == otherPrimitive->kind;
    }
    return false;
}

} // namespace types
} // namespace chtholly
