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
};
