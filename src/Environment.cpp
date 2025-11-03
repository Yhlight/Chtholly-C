#include "Environment.h"
#include <stdexcept>

Environment::Environment() : enclosing(nullptr) {}

Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(std::move(enclosing)) {}

void Environment::define(const std::string& name, bool isMutable) {
    values[name] = isMutable;
}

void Environment::assign(const Token& name, bool isMutable) {
    if (values.count(name.lexeme)) {
        values[name.lexeme] = isMutable;
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, isMutable);
        return;
    }

    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

bool Environment::isDefined(const std::string& name) {
    if (values.count(name)) {
        return true;
    }

    if (enclosing != nullptr) {
        return enclosing->isDefined(name);
    }

    return false;
}

bool Environment::isMutable(const std::string& name) {
    if (values.count(name)) {
        return values[name];
    }

    if (enclosing != nullptr) {
        return enclosing->isMutable(name);
    }

    // This should not be reached if isDefined is checked first.
    return false;
}
