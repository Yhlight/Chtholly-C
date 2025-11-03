#pragma once

#include "Token.h"
#include <map>
#include <string>
#include <memory>

class Environment {
public:
    Environment();
    Environment(std::shared_ptr<Environment> enclosing);

    void define(const std::string& name, bool isMutable);
    void assign(const Token& name, bool isMutable);
    bool isDefined(const std::string& name);
    bool isMutable(const std::string& name);

private:
    std::map<std::string, bool> values;
    std::shared_ptr<Environment> enclosing;
};
