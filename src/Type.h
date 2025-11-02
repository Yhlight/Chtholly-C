#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>
#include <vector>
#include <memory>

namespace chtholly {

enum class TypeKind {
    Base,
    Function,
    Generic,
    Array
};

class Type {
public:
    virtual ~Type() = default;
    virtual std::string toString() const = 0;
    virtual TypeKind getKind() const { return TypeKind::Base; }
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

class CharType : public Type {
public:
    std::string toString() const override { return "char"; }
};

class DoubleType : public Type {
public:
    std::string toString() const override { return "double"; }
};

} // namespace chtholly

#endif // CHTHOLLY_TYPE_H