#ifndef CHTHOLLY_ENVIRONMENT_H
#define CHTHOLLY_ENVIRONMENT_H

#include "Token.h"
#include <string>
#include <unordered_map>
#include <memory>

// Forward declaration for Object
class Object;

class Environment {
public:
    Environment();
    Environment(std::shared_ptr<Environment> enclosing);

    void define(const std::string& name, bool isMutable);
    void assign(const Token& name, bool isMutable);
    bool isDefined(const std::string& name);
    bool isMutable(const std::string& name);

    std::shared_ptr<Environment> enclosing;

private:
    std::unordered_map<std::string, bool> values;
};

#endif // CHTHOLLY_ENVIRONMENT_H
