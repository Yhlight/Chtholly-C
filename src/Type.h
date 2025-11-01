#pragma once

#include <string>
#include <vector>
#include <memory>

namespace chtholly {

class Type {
public:
    virtual ~Type() = default;
    virtual std::string toString() const = 0;
    virtual std::unique_ptr<Type> clone() const = 0;
};

class IntType : public Type {
public:
    std::string toString() const override { return "int"; }
    std::unique_ptr<Type> clone() const override { return std::make_unique<IntType>(); }
};

class StringType : public Type {
public:
    std::string toString() const override { return "string"; }
    std::unique_ptr<Type> clone() const override { return std::make_unique<StringType>(); }
};

class BoolType : public Type {
public:
    std::string toString() const override { return "bool"; }
    std::unique_ptr<Type> clone() const override { return std::make_unique<BoolType>(); }
};

class VoidType : public Type {
public:
    std::string toString() const override { return "void"; }
    std::unique_ptr<Type> clone() const override { return std::make_unique<VoidType>(); }
};

class FunctionType : public Type {
public:
    FunctionType(std::vector<std::unique_ptr<Type>> paramTypes, std::unique_ptr<Type> returnType)
        : paramTypes(std::move(paramTypes)), returnType(std::move(returnType)) {}

    std::unique_ptr<Type> clone() const override {
        std::vector<std::unique_ptr<Type>> clonedParams;
        for (const auto& param : paramTypes) {
            clonedParams.push_back(param->clone());
        }
        return std::make_unique<FunctionType>(std::move(clonedParams), returnType->clone());
    }

    std::string toString() const override {
        std::string str = "func(";
        for (size_t i = 0; i < paramTypes.size(); ++i) {
            str += paramTypes[i]->toString();
            if (i < paramTypes.size() - 1) {
                str += ", ";
            }
        }
        str += ") -> " + returnType->toString();
        return str;
    }

    const std::vector<std::unique_ptr<Type>>& getParamTypes() const { return paramTypes; }
    const Type* getReturnType() const { return returnType.get(); }

private:
    std::vector<std::unique_ptr<Type>> paramTypes;
    std::unique_ptr<Type> returnType;
};

class StructType : public Type {
public:
    struct Member {
        std::string name;
        std::unique_ptr<Type> type;
    };

    StructType(std::string name, std::vector<Member> members)
        : name(std::move(name)), members(std::move(members)) {}

    std::unique_ptr<Type> clone() const override {
        std::vector<Member> clonedMembers;
        for (const auto& member : members) {
            clonedMembers.push_back({member.name, member.type->clone()});
        }
        return std::make_unique<StructType>(name, std::move(clonedMembers));
    }

    std::string toString() const override { return name; }
    const std::string& getName() const { return name; }
    const std::vector<Member>& getMembers() const { return members; }

private:
    std::string name;
    std::vector<Member> members;
};

} // namespace chtholly
