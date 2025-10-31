#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>

class Type {
public:
    virtual ~Type() = default;
    virtual std::string toString() const = 0;
};

class PrimitiveType : public Type {
public:
    PrimitiveType(const std::string& name) : name_(name) {}
    std::string toString() const override { return name_; }

private:
    std::string name_;
};

#endif // CHTHOLLY_TYPE_H
