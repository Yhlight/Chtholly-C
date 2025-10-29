#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>
#include <vector>
#include <memory>

namespace Chtholly {

class Type {
public:
    enum TypeKind {
        Primitive,
        Struct,
        Function
    };

    Type(TypeKind kind);
    virtual ~Type() = default;

    TypeKind getKind() const { return kind; }
    virtual std::string toString() const = 0;

private:
    TypeKind kind;
};

class PrimitiveType : public Type {
public:
    enum PrimitiveKind {
        Int,
        Double,
        Bool,
        String,
        Char,
        Void
    };

    PrimitiveType(PrimitiveKind primitiveKind);

    PrimitiveKind getPrimitiveKind() const { return primitiveKind; }
    std::string toString() const override;

    static bool classof(const Type* t) { return t->getKind() == Type::Primitive; }

private:
    PrimitiveKind primitiveKind;
};

class TypeSystem {
public:
    TypeSystem();

    PrimitiveType* getIntType() const { return intType.get(); }
    PrimitiveType* getDoubleType() const { return doubleType.get(); }
    PrimitiveType* getBoolType() const { return boolType.get(); }
    PrimitiveType* getStringType() const { return stringType.get(); }
    PrimitiveType* getCharType() const { return charType.get(); }
    PrimitiveType* getVoidType() const { return voidType.get(); }

private:
    std::unique_ptr<PrimitiveType> intType;
    std::unique_ptr<PrimitiveType> doubleType;
    std::unique_ptr<PrimitiveType> boolType;
    std::unique_ptr<PrimitiveType> stringType;
    std::unique_ptr<PrimitiveType> charType;
    std::unique_ptr<PrimitiveType> voidType;
};

} // namespace Chtholly

#endif // CHTHOLLY_TYPE_H
