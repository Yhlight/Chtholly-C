#pragma once

#include <string>
#include <vector>
#include <memory>

class Type {
public:
    virtual ~Type() = default;
    virtual std::string to_string() const = 0;
};

class IntType : public Type {
public:
    std::string to_string() const override { return "int"; }
};

class DoubleType : public Type {
public:
    std::string to_string() const override { return "double"; }
};

class StringType : public Type {
public:
    std::string to_string() const override { return "string"; }
};

class VoidType : public Type {
public:
    std::string to_string() const override { return "void"; }
};

class BoolType : public Type {
public:
    std::string to_string() const override { return "bool"; }
};

class ArrayType : public Type {
    std::shared_ptr<Type> element_type_;
    int size_; // -1 indicates no size specified

public:
    ArrayType(std::shared_ptr<Type> element_type, int size = -1)
        : element_type_(std::move(element_type)), size_(size) {}

    std::shared_ptr<Type> get_element_type() const { return element_type_; }
    int get_size() const { return size_; }

    std::string to_string() const override {
        if (size_ == -1) {
            return "array[" + element_type_->to_string() + "]";
        }
        return "array[" + element_type_->to_string() + "; " + std::to_string(size_) + "]";
    }
};
