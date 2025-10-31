#include "Type.h"

bool Type::isEqual(const Type& other) const {
    return this->getKind() == other.getKind() && this->name == other.name;
}

PrimitiveType::PrimitiveType(Kind kind) : Type(""), kind_(kind) {
    switch (kind) {
        case Kind::Int: name = "int"; break;
        case Kind::UInt: name = "unsigned int"; break;
        case Kind::Char: name = "char"; break;
        case Kind::Double: name = "double"; break;
        case Kind::Bool: name = "bool"; break;
        case Kind::String: name = "std::string"; break;
        case Kind::Void: name = "void"; break;
    }
}

bool ReferenceType::isEqual(const Type& other) const {
    if (other.getKind() != TypeKind::Reference) {
        return false;
    }
    auto& otherRef = static_cast<const ReferenceType&>(other);
    return this->isMutable == otherRef.isMutable && this->referencedType->isEqual(*otherRef.referencedType);
}
