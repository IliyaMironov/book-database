#pragma once

#include <string>
#include <string_view>
#include <functional>

namespace bookdb {

// Прозрачное лексикографическое сравнение строк
struct TransparentStringLess {
    using is_transparent = void;

    constexpr bool operator()(std::string_view lhs,
                              std::string_view rhs) const noexcept {
        return lhs < rhs;
    }
};

// Прозрачная проверка эквивалентности строк
struct TransparentStringEqual {
    using is_transparent = void;

    constexpr bool operator()(std::string_view lhs,
                              std::string_view rhs) const noexcept {
        return lhs == rhs;
    }
};

// Прозрачный хэш строк
struct TransparentStringHash {
    using is_transparent = void;

    constexpr std::size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }
};

} // namespace bookdb
