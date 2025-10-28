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
