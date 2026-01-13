#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename I>
concept BookIterator =
    std::input_iterator<I> &&
    std::same_as<
        std::remove_cvref_t<std::iter_reference_t<I>>,
        Book
    >;

template <typename S, typename I>
concept BookSentinel =
    std::sentinel_for<S, I> &&
    BookIterator<I>;

template <typename T>
concept BookContainerLike =
    std::ranges::range<T> &&
    BookIterator<std::ranges::iterator_t<T>> &&
    requires (T c) {
        typename T::value_type;
        requires std::same_as<std::remove_cvref_t<typename T::value_type>, Book>;
        { c.size() } -> std::convertible_to<std::size_t>;
        { c.begin() };
        { c.end() };
        { c.clear() };
        { c.emplace_back(std::declval<const Book&>()) };
    };


template <typename P>
concept BookPredicate =
    std::predicate<P, const Book&>;

template <typename C>
concept BookComparator =
    std::strict_weak_order<C, const Book&, const Book&>;

}  // namespace bookdb
