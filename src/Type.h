#pragma once

#include <string>
#include <vector>
#include <memory>

namespace chtholly {

class Type {
public:
    virtual ~Type() = default;
    virtual std::string toString() const = 0;
};

class IntType : public Type {
public:
    std::string toString() const override { return "int"; }
};

class StringType : public Type {
public:
    std::string toString() const override { return "string"; }
};

class BoolType : public Type {
public:
    std::string toString() const override { return "bool"; }
};

class VoidType : public Type {
public:
    std::string toString() const override { return "void"; }
};

// More complex types to be added later, e.g., FunctionType, StructType

} // namespace chtholly
