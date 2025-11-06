#pragma once

#include <variant>
#include <optional>
#include <stdexcept>

template<typename T, typename E>
class Result {
public:
    // Static factory functions
    static Result<T, E> pass(T value) {
        return Result(std::in_place_index<0>, value);
    }

    static Result<T, E> fail(E error) {
        return Result(std::in_place_index<1>, error);
    }

    // Observers
    bool is_pass() const {
        return m_variant.index() == 0;
    }

    bool is_fail() const {
        return m_variant.index() == 1;
    }

    // Value accessors
    T unwarp() const {
        if (!is_pass()) {
            throw std::runtime_error("Called unwarp on a fail result");
        }
        return std::get<0>(m_variant);
    }

    E unwarp_err() const {
        if (!is_fail()) {
            throw std::runtime_error("Called unwarp_err on a pass result");
        }
        return std::get<1>(m_variant);
    }

private:
    template<typename... Args>
    Result(Args&&... args) : m_variant(std::forward<Args>(args)...) {}

    std::variant<T, E> m_variant;
};
