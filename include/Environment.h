#ifndef CHTHOLLY_ENVIRONMENT_H
#define CHTHOLLY_ENVIRONMENT_H

#include <map>
#include <string>
#include <memory>
#include <any>
#include "Token.h"

class Environment : public std::enable_shared_from_this<Environment> {
public:
    explicit Environment(std::shared_ptr<Environment> enclosing = nullptr);

    void define(const std::string& name, std::any value, bool isMutable);
    void assign(const Token& name, std::any value);
    std::any get(const Token& name);

    bool isMutable(const std::string& name);

    std::shared_ptr<Environment> enclosing;

private:
    std::map<std::string, std::any> values;
    std::map<std::string, bool> mutability;
};

#endif //CHTHOLLY_ENVIRONMENT_H
