#include "Type.h"

bool Type::isEqual(const Type& other) const {
    if (this->getKind() != other.getKind()) {
        return false;
    }

    if (this->getKind() == TypeKind::Reference) {
        auto thisRef = static_cast<const ReferenceType*>(this);
        auto otherRef = static_cast<const ReferenceType*>(&other);
        return thisRef->isMutable() == otherRef->isMutable() && thisRef->getReferencedType()->isEqual(*otherRef->getReferencedType());
    }

    return this->name == other.name;
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
