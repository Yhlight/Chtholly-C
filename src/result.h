#ifndef CHTHOLLY_RESULT_H
#define CHTHOLLY_RESULT_H

#include <variant>
#include <stdexcept>
#include <utility>

template<typename T, typename E>
class result {
public:
    // Factory functions
    static result<T, E> pass(T value) {
        return result(std::in_place_index<0>, value);
    }

    static result<T, E> fail(E error) {
        return result(std::in_place_index<1>, error);
    }

    // Methods
    bool is_pass() const {
        return m_variant.index() == 0;
    }

    bool is_fail() const {
        return m_variant.index() == 1;
    }

    T unwarp() {
        if (is_fail()) {
            throw std::runtime_error("Called unwarp on a fail result");
        }
        return std::get<0>(m_variant);
    }

    E unwarp_err() {
        if (is_pass()) {
            throw std::runtime_error("Called unwarp_err on a pass result");
        }
        return std::get<1>(m_variant);
    }

private:
    template<typename... Args>
    result(Args&&... args) : m_variant(std::forward<Args>(args)...) {}

    std::variant<T, E> m_variant;
};

#endif // CHTHOLLY_RESULT_H
