#pragma once

#include <string>
#include <memory>
#include <vector>

namespace chtholly {

enum class TypeKind {
    Int,
    Double,
    Bool,
    Char,
    String,
    Void,
    Array,
    Struct
};

class Type {
public:
    virtual ~Type() = default;
    virtual TypeKind getKind() const = 0;
    virtual std::string toString() const = 0;
};

class IntType : public Type {
public:
    TypeKind getKind() const override { return TypeKind::Int; }
    std::string toString() const override { return "int"; }
};

class DoubleType : public Type {
public:
    TypeKind getKind() const override { return TypeKind::Double; }
    std::string toString() const override { return "double"; }
};

class BoolType : public Type {
public:
    TypeKind getKind() const override { return TypeKind::Bool; }
    std::string toString() const override { return "bool"; }
};

class CharType : public Type {
public:
    TypeKind getKind() const override { return TypeKind::Char; }
    std::string toString() const override { return "char"; }
};

class StringType : public Type {
public:
    TypeKind getKind() const override { return TypeKind::String; }
    std::string toString() const override { return "string"; }
};

class VoidType : public Type {
public:
    TypeKind getKind() const override { return TypeKind::Void; }
    std::string toString() const override { return "void"; }
};

class ArrayType : public Type {
    std::shared_ptr<Type> elementType;
public:
    ArrayType(std::shared_ptr<Type> elementType) : elementType(elementType) {}
    TypeKind getKind() const override { return TypeKind::Array; }
    std::string toString() const override { return "array[" + elementType->toString() + "]"; }
};

} // namespace chtholly
