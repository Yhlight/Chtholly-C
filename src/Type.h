#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>
#include <memory>
#include <vector>

enum class TypeKind {
    Primitive,
    Array,
    Struct,
    Function
};

class Type {
public:
    explicit Type(std::string name) : name(std::move(name)) {}
    virtual ~Type() = default;
    virtual TypeKind getKind() const = 0;
    std::string toString() const { return name; }
    virtual bool isEqual(const Type& other) const;

    std::string name;
};

class PrimitiveType : public Type {
public:
    enum class Kind {
        Int,
        UInt,
        Char,
        Double,
        Bool,
        String,
        Void
    };

    explicit PrimitiveType(Kind kind);

    TypeKind getKind() const override { return TypeKind::Primitive; }
    Kind getPrimitiveKind() const { return kind_; }
    bool isEqual(const Type& other) const override;

private:
    Kind kind_;
};

class FunctionType : public Type {
public:
    FunctionType(std::shared_ptr<Type> returnType, std::vector<std::shared_ptr<Type>> parameterTypes)
        : Type("function"), returnType(std::move(returnType)), parameterTypes(std::move(parameterTypes)) {}

    TypeKind getKind() const override { return TypeKind::Function; }
    bool isEqual(const Type& other) const override;

    std::shared_ptr<Type> returnType;
    std::vector<std::shared_ptr<Type>> parameterTypes;
};

#endif // CHTHOLLY_TYPE_H
