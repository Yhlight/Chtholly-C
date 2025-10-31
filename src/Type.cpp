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

bool PrimitiveType::isEqual(const Type& other) const {
    if (other.getKind() != TypeKind::Primitive) {
        return false;
    }
    return this->kind_ == static_cast<const PrimitiveType&>(other).kind_;
}

bool FunctionType::isEqual(const Type& other) const {
    if (other.getKind() != TypeKind::Function) {
        return false;
    }
    const auto& otherFunc = static_cast<const FunctionType&>(other);
    if (!this->returnType->isEqual(*otherFunc.returnType)) {
        return false;
    }
    if (this->parameterTypes.size() != otherFunc.parameterTypes.size()) {
        return false;
    }
    for (size_t i = 0; i < this->parameterTypes.size(); ++i) {
        if (!this->parameterTypes[i]->isEqual(*otherFunc.parameterTypes[i])) {
            return false;
        }
    }
    return true;
}
