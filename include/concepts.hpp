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
    BookIterator<std::ranges::iterator_t<T>>;

template <typename P>
concept BookPredicate =
    std::predicate<P, const Book&>;

template <typename C>
concept BookComparator =
    std::strict_weak_order<C, const Book&, const Book&>;

}  // namespace bookdb
