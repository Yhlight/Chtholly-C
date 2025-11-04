#include <stdexcept>
#include "Environment.h"
#include "Token.h"

Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(std::move(enclosing)) {}

void Environment::define(const std::string& name, std::any value, bool isMutable) {
    values[name] = std::move(value);
    mutability[name] = isMutable;
}

void Environment::assign(const Token& name, std::any value) {
    if (values.count(name.lexeme)) {
        if (!mutability[name.lexeme]) {
            throw std::runtime_error("Cannot assign to immutable variable '" + name.lexeme + "'.");
        }
        values[name.lexeme] = std::move(value);
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, std::move(value));
        return;
    }

    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

std::any Environment::get(const Token& name) {
    if (values.count(name.lexeme)) {
        return values[name.lexeme];
    }

    if (enclosing != nullptr) {
        return enclosing->get(name);
    }

    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

bool Environment::isMutable(const std::string& name) {
    if (mutability.count(name)) {
        return mutability[name];
    }

    if (enclosing != nullptr) {
        return enclosing->isMutable(name);
    }

    // Should not be reached if the variable is defined
    return false;
}
