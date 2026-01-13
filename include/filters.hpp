#pragma once

#include <algorithm>
#include <functional>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {

constexpr inline auto YearBetween(int from, int to) {
    return [from, to](const Book& b) {
        return b.year >= from && b.year <= to;
    };
}

constexpr inline auto RatingAbove(double min_rating) {
    return [min_rating](const Book& b) {
        return b.rating > min_rating;
    };
}

constexpr inline auto GenreIs(Genre g) {
    return [g](const Book& b) {
        return b.genre == g;
    };
}

template <typename... Preds>
auto all_of(Preds&&... preds) {
    auto ps = std::make_tuple(std::move(preds)...);
    return [ps = std::move(ps)](const Book& b) {
        return std::apply([&b](const auto&... f) { return (f(b) && ...); }, ps);
    };
}

template <typename... Preds>
auto any_of(Preds&&... preds) {
    auto ps = std::make_tuple(std::move(preds)...);
    return [ps = std::move(ps)](const Book& b) {
        return std::apply([&b](const auto&... f) { return (f(b) || ...); }, ps);
    };
}

template <typename It, typename Sentinel, typename Predicate>
auto filterBooks(It first, Sentinel last, Predicate pred)
    -> std::vector<std::reference_wrapper<const Book>>
{
    std::vector<std::reference_wrapper<const Book>> result;

    for (; first != last; ++first) {
        if (pred(*first)) {
            result.emplace_back(*first);
        }
    }

    return result;
}

}  // namespace bookdb
