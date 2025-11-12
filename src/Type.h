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
    STRUCT,
    BORROW
};

class Type {
public:
    virtual ~Type() = default;
    virtual std::string to_string() const = 0;
    virtual TypeKind get_kind() const = 0;
    virtual bool equals(const Type& other) const = 0;
};

class BasicType : public Type {
public:
    explicit BasicType(const std::string& name) : name(name) {}
    std::string to_string() const override { return name; }
    TypeKind get_kind() const override { return TypeKind::BASIC; }
    const std::string& get_name() const { return name; }

    bool equals(const Type& other) const override {
        if (get_kind() != other.get_kind()) {
            return false;
        }
        const auto& other_basic = static_cast<const BasicType&>(other);
        return get_name() == other_basic.get_name();
    }
private:
    std::string name;
};

class FunctionType : public Type {
public:
    FunctionType(std::shared_ptr<Type> return_type, const std::vector<std::shared_ptr<Type>>& parameter_types)
        : return_type(std::move(return_type)), parameter_types(parameter_types) {}

    std::string to_string() const override {
        std::string s = "function(";
        for (size_t i = 0; i < parameter_types.size(); ++i) {
            s += parameter_types[i]->to_string();
            if (i < parameter_types.size() - 1) {
                s += ", ";
            }
        }
        s += ") -> ";
        s += return_type->to_string();
        return s;
    }
    TypeKind get_kind() const override { return TypeKind::FUNCTION; }

    const std::shared_ptr<Type>& get_return_type() const { return return_type; }
    const std::vector<std::shared_ptr<Type>>& get_parameter_types() const { return parameter_types; }

    bool equals(const Type& other) const override {
        if (get_kind() != other.get_kind()) {
            return false;
        }
        const auto& other_func = static_cast<const FunctionType&>(other);
        if (!get_return_type()->equals(*other_func.get_return_type())) {
            return false;
        }
        if (get_parameter_types().size() != other_func.get_parameter_types().size()) {
            return false;
        }
        for (size_t i = 0; i < get_parameter_types().size(); ++i) {
            if (!get_parameter_types()[i]->equals(*other_func.get_parameter_types()[i])) {
                return false;
            }
        }
        return true;
    }

private:
    std::shared_ptr<Type> return_type;
    std::vector<std::shared_ptr<Type>> parameter_types;
};

class ArrayType : public Type {
public:
    explicit ArrayType(std::shared_ptr<Type> element_type) : element_type(element_type) {}
    std::string to_string() const override {
        return "array[" + element_type->to_string() + "]";
    }
    TypeKind get_kind() const override { return TypeKind::ARRAY; }

    bool equals(const Type& other) const override {
        if (get_kind() != other.get_kind()) {
            return false;
        }
        const auto& other_array = static_cast<const ArrayType&>(other);
        return element_type->equals(*other_array.element_type);
    }

private:
    std::shared_ptr<Type> element_type;
};

class StructType : public Type {
public:
    explicit StructType(const std::string& name) : name(name) {}
    std::string to_string() const override { return name; }
    TypeKind get_kind() const override { return TypeKind::STRUCT; }
    const std::string& get_name() const { return name; }

    bool equals(const Type& other) const override {
        if (get_kind() != other.get_kind()) {
            return false;
        }
        const auto& other_struct = static_cast<const StructType&>(other);
        return get_name() == other_struct.get_name();
    }

private:
    std::string name;
};

class BorrowType : public Type {
public:
    BorrowType(std::shared_ptr<Type> element_type, bool is_mutable)
        : element_type(std::move(element_type)), is_mutable(is_mutable) {}

    std::string to_string() const override {
        return std::string("&") + (is_mutable ? "mut " : "") + element_type->to_string();
    }

    TypeKind get_kind() const override { return TypeKind::BORROW; }

    bool equals(const Type& other) const override {
        if (get_kind() != other.get_kind()) {
            return false;
        }
        const auto& other_borrow = static_cast<const BorrowType&>(other);
        return is_mutable == other_borrow.is_mutable &&
               element_type->equals(*other_borrow.element_type);
    }

private:
    std::shared_ptr<Type> element_type;
    bool is_mutable;
};


} // namespace chtholly

#endif // CHTHOLLY_TYPE_H
