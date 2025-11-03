#pragma once

#include "Token.h"
#include <vector>
#include <memory>

struct TypeInfo {
    Token baseType;
    bool isReference = false;
    bool isMutable = false;
    std::vector<TypeInfo> params;
    std::unique_ptr<TypeInfo> returnType;

    TypeInfo() = default;

    TypeInfo(Token baseType) : baseType(baseType) {}

    TypeInfo(const TypeInfo& other)
        : baseType(other.baseType),
          isReference(other.isReference),
          isMutable(other.isMutable),
          params(other.params) {
        if (other.returnType) {
            returnType = std::make_unique<TypeInfo>(*other.returnType);
        }
    }

    TypeInfo& operator=(const TypeInfo& other) {
        if (this == &other) {
            return *this;
        }
        baseType = other.baseType;
        isReference = other.isReference;
        isMutable = other.isMutable;
        params = other.params;
        if (other.returnType) {
            returnType = std::make_unique<TypeInfo>(*other.returnType);
        } else {
            returnType.reset();
        }
        return *this;
    }
};
