#ifndef CHTHOLLY_TYPESYSTEM_H
#define CHTHOLLY_TYPESYSTEM_H

#include <string>
#include <vector>
#include <memory>

namespace chtholly {
namespace types {

class Type {
public:
    virtual ~Type() = default;
    virtual std::string toString() const = 0;
    virtual bool isEqualTo(const Type& other) const = 0;
};

class PrimitiveType : public Type {
public:
    enum class Kind {
        INT,
        UINT,
        DOUBLE,
        BOOL,
        CHAR,
        STRING,
        VOID,
        AUTO // For type inference
    };

    Kind kind;

    explicit PrimitiveType(Kind kind) : kind(kind) {}

    std::string toString() const override;
    bool isEqualTo(const Type& other) const override;
};


class FunctionType : public Type {
public:
    std::shared_ptr<Type> return_type;
    std::vector<std::shared_ptr<Type>> param_types;

    FunctionType(std::shared_ptr<Type> return_type, std::vector<std::shared_ptr<Type>> param_types)
        : return_type(std::move(return_type)), param_types(std::move(param_types)) {}

    std::string toString() const override;
    bool isEqualTo(const Type& other) const override;
};


} // namespace types
} // namespace chtholly

#endif // CHTHOLLY_TYPESYSTEM_H
