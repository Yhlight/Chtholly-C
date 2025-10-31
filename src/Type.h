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
    Struct,
    Function,
    Enum,
    Trait
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

class FunctionType : public Type {
    std::shared_ptr<Type> returnType;
    std::vector<std::shared_ptr<Type>> paramTypes;
public:
    FunctionType(std::shared_ptr<Type> returnType, std::vector<std::shared_ptr<Type>> paramTypes)
        : returnType(returnType), paramTypes(paramTypes) {}
    TypeKind getKind() const override { return TypeKind::Function; }
    std::string toString() const override {
        std::string str = "func(";
        for (size_t i = 0; i < paramTypes.size(); ++i) {
            str += paramTypes[i]->toString();
            if (i < paramTypes.size() - 1) {
                str += ", ";
            }
        }
        str += ") -> " + returnType->toString();
        return str;
    }
    const std::shared_ptr<Type>& getReturnType() const { return returnType; }
    const std::vector<std::shared_ptr<Type>>& getParamTypes() const { return paramTypes; }
};

class EnumType : public Type {
    std::string name;
    std::vector<std::string> members;
public:
    EnumType(std::string name, std::vector<std::string> members)
        : name(std::move(name)), members(std::move(members)) {}
    TypeKind getKind() const override { return TypeKind::Enum; }
    std::string toString() const override { return name; }
    const std::string& getName() const { return name; }
    const std::vector<std::string>& getMembers() const { return members; }
};

struct MemberVarType {
    std::string name;
    std::shared_ptr<Type> type;
    bool isPublic;
};

class TraitType;

class StructType : public Type {
    std::string name;
    std::vector<MemberVarType> members;
    std::vector<std::shared_ptr<TraitType>> implementedTraits;
public:
    StructType(std::string name, std::vector<MemberVarType> members)
        : name(std::move(name)), members(std::move(members)) {}
    TypeKind getKind() const override { return TypeKind::Struct; }
    std::string toString() const override { return name; }
    const std::string& getName() const { return name; }
    const std::vector<MemberVarType>& getMembers() const { return members; }
    void addImplementedTrait(std::shared_ptr<TraitType> trait) {
        implementedTraits.push_back(trait);
    }
    const std::vector<std::shared_ptr<TraitType>>& getImplementedTraits() const {
        return implementedTraits;
    }
};

class TraitType : public Type {
    std::string name;
    std::vector<std::shared_ptr<FunctionType>> methods;
public:
    TraitType(std::string name, std::vector<std::shared_ptr<FunctionType>> methods)
        : name(std::move(name)), methods(std::move(methods)) {}
    TypeKind getKind() const override { return TypeKind::Trait; }
    std::string toString() const override { return name; }
    const std::string& getName() const { return name; }
    const std::vector<std::shared_ptr<FunctionType>>& getMethods() const { return methods; }
};

} // namespace chtholly
