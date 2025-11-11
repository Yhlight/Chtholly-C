#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>
#include <vector>
#include <memory>

namespace chtholly {

enum class TypeKind {
    BASIC,
    FUNCTION,
    ARRAY,
    STRUCT
};

class Type {
public:
    virtual ~Type() = default;
    virtual std::string to_string() const = 0;
    virtual TypeKind get_kind() const = 0;
};

class BasicType : public Type {
public:
    explicit BasicType(const std::string& name) : name(name) {}
    std::string to_string() const override { return name; }
    TypeKind get_kind() const override { return TypeKind::BASIC; }
    const std::string& get_name() const { return name; }
private:
    std::string name;
};

class FunctionType : public Type {
public:
    FunctionType(std::shared_ptr<Type> return_type, const std::vector<std::shared_ptr<Type>>& params)
        : return_type(return_type), params(params) {}

    std::string to_string() const override {
        std::string s = "function(";
        for (size_t i = 0; i < params.size(); ++i) {
            s += params[i]->to_string();
            if (i < params.size() - 1) {
                s += ", ";
            }
        }
        s += ") -> ";
        s += return_type->to_string();
        return s;
    }
    TypeKind get_kind() const override { return TypeKind::FUNCTION; }

private:
    std::shared_ptr<Type> return_type;
    std::vector<std::shared_ptr<Type>> params;
};

class ArrayType : public Type {
public:
    explicit ArrayType(std::shared_ptr<Type> element_type) : element_type(element_type) {}
    std::string to_string() const override {
        return "array[" + element_type->to_string() + "]";
    }
    TypeKind get_kind() const override { return TypeKind::ARRAY; }

private:
    std::shared_ptr<Type> element_type;
};

class StructType : public Type {
public:
    explicit StructType(const std::string& name) : name(name) {}
    std::string to_string() const override { return name; }
    TypeKind get_kind() const override { return TypeKind::STRUCT; }
    const std::string& get_name() const { return name; }

private:
    std::string name;
};


} // namespace chtholly

#endif // CHTHOLLY_TYPE_H
