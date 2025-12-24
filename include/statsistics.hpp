#pragma once

#include <algorithm>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>

#include "book_database.hpp"

#include <print>

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T>& db, Comparator comp = {}) {
    using result_type = std::vector<std::pair<std::string_view, std::size_t>>;

    result_type result;

    for (const auto& book : db) {
        result.emplace_back(book.author, 1);
    }

    std::ranges::sort(result, [&](auto& a, auto& b) {
        return comp(a.first, b.first);
    });

    auto out = result.begin();
    for (auto it = result.begin(); it != result.end(); ) {
        std::string_view author = it->first;
        std::size_t count = 0;

        while (it != result.end() && !comp(author, it->first) && !comp(it->first, author)) {
            count += it->second;
            ++it;
        }

        *out++ = {author, count};
    }

    result.erase(out, result.end());
    return result;
}

auto calculateGenreRatings(const auto& db) {
    using entry = std::pair<Genre, double>;
    std::vector<entry> flat;

    for (const auto& book : db) {
        flat.emplace_back(book.genre, book.rating);
    }

    std::ranges::sort(flat, {}, &entry::first);

    std::vector<std::pair<Genre, double>> result;

    for (auto it = flat.begin(); it != flat.end(); ) {
        Genre g = it->first;
        double sum = 0.0;
        std::size_t count = 0;

        while (it != flat.end() && it->first == g) {
            sum += it->second;
            ++count;
            ++it;
        }

        result.emplace_back(g, sum / count);
    }

    return result;
}

double calculateAverageRating(const auto& db) {
    if (db.empty()) {
        throw std::logic_error("Empty database");
    }

    double sum = std::accumulate(
        db.begin(), db.end(), 0.0,
        [](double acc, const Book& b) {
            return acc + b.rating;
        }
    );

    return sum / db.size();
}


template <BookContainerLike T>
auto sampleRandomBooks(const BookDatabase<T>& db, std::size_t count) {
    if (count > db.size()) {
        throw std::out_of_range("Sample size exceeds database size");
    }

    std::vector<std::reference_wrapper<const Book>> result;
    result.reserve(count);

    std::vector<std::size_t> indices(db.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::mt19937 gen{std::random_device{}()};
    std::ranges::shuffle(indices, gen);

    auto it = db.begin();
    for (std::size_t i = 0; i < count; ++i) {
        result.emplace_back(*(std::next(it, indices[i])));
    }

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
