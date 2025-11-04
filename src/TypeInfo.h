#ifndef CHTHOLLY_TYPEINFO_H
#define CHTHOLLY_TYPEINFO_H

#include "Token.h"
#include <vector>
#include <memory>
#include <utility>

namespace chtholly {

struct TypeInfo {
    TokenType base_type;
    bool is_ref = false;
    bool is_mut_ref = false;

    // For function types
    std::unique_ptr<TypeInfo> return_type;
    std::vector<std::unique_ptr<TypeInfo>> params;

    // For array types
    std::unique_ptr<TypeInfo> array_element_type;

    // For generic types
    std::vector<std::unique_ptr<TypeInfo>> generic_args;

    explicit TypeInfo(TokenType base_type) : base_type(base_type) {}

    // Copy constructor
    TypeInfo(const TypeInfo& other) {
        base_type = other.base_type;
        is_ref = other.is_ref;
        is_mut_ref = other.is_mut_ref;

        if (other.return_type) {
            return_type = std::make_unique<TypeInfo>(*other.return_type);
        }
        for (const auto& param : other.params) {
            params.push_back(std::make_unique<TypeInfo>(*param));
        }
        if (other.array_element_type) {
            array_element_type = std::make_unique<TypeInfo>(*other.array_element_type);
        }
        for (const auto& arg : other.generic_args) {
            generic_args.push_back(std::make_unique<TypeInfo>(*arg));
        }
    }

    // Copy assignment operator
    TypeInfo& operator=(const TypeInfo& other) {
        if (this == &other) {
            return *this;
        }

        base_type = other.base_type;
        is_ref = other.is_ref;
        is_mut_ref = other.is_mut_ref;

        if (other.return_type) {
            return_type = std::make_unique<TypeInfo>(*other.return_type);
        } else {
            return_type.reset();
        }

        params.clear();
        for (const auto& param : other.params) {
            params.push_back(std::make_unique<TypeInfo>(*param));
        }

        if (other.array_element_type) {
            array_element_type = std::make_unique<TypeInfo>(*other.array_element_type);
        } else {
            array_element_type.reset();
        }

        generic_args.clear();
        for (const auto& arg : other.generic_args) {
            generic_args.push_back(std::make_unique<TypeInfo>(*arg));
        }

        return *this;
    }
};

} // namespace chtholly

#endif // CHTHOLLY_TYPEINFO_H
