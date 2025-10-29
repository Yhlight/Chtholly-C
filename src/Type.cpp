#include "Type.h"

namespace Chtholly {

Type::Type(TypeKind kind) : kind(kind) {}

PrimitiveType::PrimitiveType(PrimitiveKind primitiveKind)
    : Type(Type::Primitive), primitiveKind(primitiveKind) {}

std::string PrimitiveType::toString() const {
    switch (primitiveKind) {
        case Int: return "int";
        case Double: return "double";
        case Bool: return "bool";
        case String: return "string";
        case Char: return "char";
        case Void: return "void";
    }
    return "unknown";
}

TypeSystem::TypeSystem() {
    intType = std::make_unique<PrimitiveType>(PrimitiveType::Int);
    doubleType = std::make_unique<PrimitiveType>(PrimitiveType::Double);
    boolType = std::make_unique<PrimitiveType>(PrimitiveType::Bool);
    stringType = std::make_unique<PrimitiveType>(PrimitiveType::String);
    charType = std::make_unique<PrimitiveType>(PrimitiveType::Char);
    voidType = std::make_unique<PrimitiveType>(PrimitiveType::Void);
}

} // namespace Chtholly
