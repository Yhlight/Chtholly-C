#include "Type.h"

std::string PrimitiveType::toString() const {
    switch (kind_) {
        case Kind::Int: return "int";
        case Kind::UInt: return "unsigned int";
        case Kind::Char: return "char";
        case Kind::Double: return "double";
        case Kind::Bool: return "bool";
        case Kind::String: return "std::string";
        case Kind::Void: return "void";
    }
    return "unknown";
}
