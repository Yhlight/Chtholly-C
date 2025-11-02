#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>
#include <vector>
#include <memory>

enum class TypeKind {
    PRIMITIVE,
    STRUCT,
    FUNCTION,
    ARRAY
};

struct Type {
    TypeKind kind;
    bool is_ref = false;
    bool is_mut_ref = false;

    explicit Type(TypeKind kind) : kind(kind) {}
    virtual ~Type() = default;
    virtual std::shared_ptr<Type> clone() const = 0;
};

struct PrimitiveType : public Type {
    std::string name;
    explicit PrimitiveType(std::string name) : Type(TypeKind::PRIMITIVE), name(std::move(name)) {}

    std::shared_ptr<Type> clone() const override {
        return std::make_shared<PrimitiveType>(*this);
    }
};

struct ArrayType : public Type {
    std::shared_ptr<Type> element_type;
    explicit ArrayType(std::shared_ptr<Type> element_type) : Type(TypeKind::ARRAY), element_type(std::move(element_type)) {}

    std::shared_ptr<Type> clone() const override {
        return std::make_shared<ArrayType>(*this);
    }
};

#endif //CHTHOLLY_TYPE_H
