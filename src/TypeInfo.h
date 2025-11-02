#pragma once

#include "Token.h"

struct TypeInfo {
    Token baseType;
    bool isReference = false;
    bool isMutable = false;
};
