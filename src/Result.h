#ifndef CHTHOLLY_RESULT_H
#define CHTHOLLY_RESULT_H

#include <variant>
#include <optional>
#include <iostream>

template<typename T, typename E>
class Result {
public:
    // Factory functions for creating Result objects
    static Result<T, E> pass(T value) {
        return Result(std::in_place_index<0>, value);
    }

    static Result<T, E> fail(E error) {
        return Result(std::in_place_index<1>, error);
    }

    // Methods to check the state of the Result
    bool is_pass() const {
        return m_value.index() == 0;
    }

    bool is_fail() const {
        return m_value.index() == 1;
    }

    // Methods to get the value or error
    // These will throw if the Result is in the wrong state
    T pass_value() const {
        return std::get<0>(m_value);
    }

    E fail_value() const {
        return std::get<1>(m_value);
    }

private:
    std::variant<T, E> m_value;

    // Private constructor to force use of factory functions
    template<size_t I, typename... Args>
    Result(std::in_place_index_t<I> index, Args&&... args)
        : m_value(index, std::forward<Args>(args)...) {}
};

#endif // CHTHOLLY_RESULT_H
