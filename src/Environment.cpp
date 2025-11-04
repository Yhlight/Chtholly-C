#include <Environment.h>

Environment::Environment() : enclosing(nullptr) {}

Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

void Environment::define(const std::string& name, bool isMutable) {
    values[name] = isMutable;
}

void Environment::assign(const Token& name, bool isMutable) {
    if (values.find(name.lexeme) != values.end()) {
        values[name.lexeme] = isMutable;
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, isMutable);
        return;
    }

    // This should be caught by the semantic analyzer before this point.
    // Throwing an error here is a safeguard.
    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

bool Environment::isDefined(const std::string& name) {
    if (values.find(name) != values.end()) {
        return true;
    }

    if (enclosing != nullptr) {
        return enclosing->isDefined(name);
    }

    return false;
}

bool Environment::isMutable(const std::string& name) {
    if (values.find(name) != values.end()) {
        return values[name];
    }

    if (enclosing != nullptr) {
        return enclosing->isMutable(name);
    }

    // This should be caught by the semantic analyzer before this point.
    return false;
}
