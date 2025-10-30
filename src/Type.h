#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>
#include <memory>

enum class TypeKind {
    Primitive,
    Array,
    Struct
};

class Type {
public:
    virtual ~Type() = default;
    virtual TypeKind getKind() const = 0;
    virtual std::string toString() const = 0;
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

    explicit PrimitiveType(Kind kind) : kind_(kind) {}

    TypeKind getKind() const override { return TypeKind::Primitive; }
    std::string toString() const override;
    Kind getPrimitiveKind() const { return kind_; }

private:
    Kind kind_;
};

#endif // CHTHOLLY_TYPE_H
