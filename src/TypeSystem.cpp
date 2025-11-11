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


std::string FunctionType::toString() const {
    std::string result = "function(";
    for (size_t i = 0; i < param_types.size(); ++i) {
        result += param_types[i]->toString();
        if (i < param_types.size() - 1) {
            result += ", ";
        }
    }
    result += ") -> ";
    result += return_type->toString();
    return result;
}

bool FunctionType::isEqualTo(const Type& other) const {
    if (auto otherFunc = dynamic_cast<const FunctionType*>(&other)) {
        if (!return_type->isEqualTo(*otherFunc->return_type)) {
            return false;
        }
        if (param_types.size() != otherFunc->param_types.size()) {
            return false;
        }
        for (size_t i = 0; i < param_types.size(); ++i) {
            if (!param_types[i]->isEqualTo(*otherFunc->param_types[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

} // namespace types
} // namespace chtholly
