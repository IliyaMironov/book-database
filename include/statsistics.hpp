#pragma once

#include <algorithm>
#include <flat_map>
#include <iterator>
#include <numeric>
#include <random>
#include <ranges>
#include <stdexcept>
#include <string_view>

#include "book_database.hpp"

#include <print>

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T>& db, Comparator comp = {}) {
    using result_type = std::flat_map<std::string_view, std::size_t, Comparator>;

    result_type result(comp);

    for (const auto& book : db) {
        auto [it, inserted] = result.try_emplace(book.author, 0);
        ++it->second;
    }

    return result;
}

auto calculateGenreRatings(const auto& db) {
    using rating_data = std::pair<double, std::size_t>; // sum, count
    std::flat_map<Genre, rating_data> genre_stats;

    for (const auto& book : db) {
        auto [it, inserted] = genre_stats.try_emplace(book.genre, rating_data{0.0, 0});
        it->second.first += book.rating;
        ++it->second.second;
    }

    std::vector<std::pair<Genre, double>> result;
    result.reserve(genre_stats.size());

    for (const auto& [genre, stats] : genre_stats) {
        result.emplace_back(genre, stats.first / stats.second);
    }

    return result;
}

double calculateAverageRating(const auto& db) {
    if (db.empty()) {
        throw std::logic_error("Empty database");
    }

    double sum = std::transform_reduce(
        db.begin(), db.end(),
        0.0,
        std::plus{},
        [](const Book& b) {
            return b.rating;
        }
    );

    return sum / db.size();
}


template <BookContainerLike T>
auto sampleRandomBooks(const BookDatabase<T>& db, std::size_t count) {
    if (count > db.size()) {
        throw std::out_of_range("Sample size exceeds database size");
    }

    std::vector<std::reference_wrapper<const Book>> all_books;
    all_books.reserve(db.size());

    for (const auto& book : db) {
        all_books.emplace_back(book);
    }

    std::vector<std::reference_wrapper<const Book>> result;
    result.reserve(count);

    std::mt19937 gen{std::random_device{}()};
    std::ranges::sample(all_books, std::back_inserter(result), count, gen);

    return result;
}

template <BookContainerLike T, BookComparator Comp>
auto getTopNBy(BookDatabase<T>& db, std::size_t n, Comp comp) {
    if (n > db.size()) {
        throw std::out_of_range("N exceeds database size");
    }

    auto& books = const_cast<T&>(db.GetBooks());

    std::ranges::nth_element(
        books,
        books.begin() + n,
        comp
    );

    std::vector<std::reference_wrapper<const Book>> result;
    result.reserve(n);

    for (std::size_t i = 0; i < n; ++i) {
        result.emplace_back(books[i]);
    }

    std::ranges::sort(result, [&](const Book& a, const Book& b) {
        return comp(a, b);
    });

    return result;
}   

}  // namespace bookdb
