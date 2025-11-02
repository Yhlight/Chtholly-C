#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>
#include <vector>
#include <memory>

enum class TypeKind {
    PRIMITIVE,
    STRUCT,
    FUNCTION
};

struct Type {
    TypeKind kind;
    explicit Type(TypeKind kind) : kind(kind) {}
    virtual ~Type() = default;
};

struct PrimitiveType : public Type {
    std::string name;
    explicit PrimitiveType(std::string name) : Type(TypeKind::PRIMITIVE), name(std::move(name)) {}
};

#endif //CHTHOLLY_TYPE_H
