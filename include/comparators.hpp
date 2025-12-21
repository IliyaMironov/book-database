#pragma once

#include "book.hpp"

namespace bookdb::comp {

// Сортировка по автору (лексикографически)
struct LessByAuthor {
    constexpr bool operator()(const Book& lhs, const Book& rhs) const noexcept {
        return lhs.author < rhs.author;
    }
};

// Сортировка по названию
struct LessByTitle {
    constexpr bool operator()(const Book& lhs, const Book& rhs) const noexcept {
        return lhs.title < rhs.title;
    }
};

// Сортировка по году издания
struct LessByYear {
    constexpr bool operator()(const Book& lhs, const Book& rhs) const noexcept {
        return lhs.year < rhs.year;
    }
};

// Сортировка по жанру
struct LessByGenre {
    constexpr bool operator()(const Book& lhs, const Book& rhs) const noexcept {
        return lhs.genre < rhs.genre;
    }
};

// Сортировка по рейтингу (по возрастанию)
struct LessByRating {
    constexpr bool operator()(const Book& lhs, const Book& rhs) const noexcept {
        return lhs.rating < rhs.rating;
    }
};

// Сортировка по количеству прочтений
struct LessByReadCount {
    constexpr bool operator()(const Book& lhs, const Book& rhs) const noexcept {
        return lhs.read_count < rhs.read_count;
    }
};

// Сортировка по популярности:
// сначала рейтинг (по убыванию), затем количество прочтений (по убыванию)
struct LessByPopularity {
    constexpr bool operator()(const Book& lhs, const Book& rhs) const noexcept {
        if (lhs.rating != rhs.rating)
            return lhs.rating > rhs.rating;  // больше — раньше
        return lhs.read_count > rhs.read_count;
    }
};

// Комбинированная сортировка: автор, год, название
struct LessByAuthorYearTitle {
    constexpr bool operator()(const Book& lhs, const Book& rhs) const noexcept {
        if (lhs.author != rhs.author)
            return lhs.author < rhs.author;
        if (lhs.year != rhs.year)
            return lhs.year < rhs.year;
        return lhs.title < rhs.title;
    }
};

}  // namespace bookdb::comp