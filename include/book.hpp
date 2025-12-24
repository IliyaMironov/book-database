#pragma once

#include <format>
#include <stdexcept>
#include <string_view>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

struct Book {
    std::string_view author;
    std::string title;

    int year;
    Genre genre;
    double rating;
    int read_count;

    // Ваш код для конструкторов здесь
    constexpr Book(std::string_view author_,
        std::string title_,
        int year_,
        Genre genre_,
        double rating_,
        int read_count_) : author(author_), 
            title(std::move(title_)),
            year(year_),
            genre(genre_),
            rating(rating_),
            read_count(read_count_) {}

    constexpr Book(std::string_view author_,
            std::string title_,
            int year_,
            std::string_view genre_str,
            double rating_,
            int read_count_) : Book(author_, 
                std::move(title_),
                year_,
                genre_from_string(genre_str),
                rating_,
                read_count_) {}

    constexpr static Genre genre_from_string(std::string_view sv) {
        using bookdb::Genre;
        if (sv == "Fiction") {
            return Genre::Fiction;
        }
        if (sv == "Mystery") {
            return Genre::Mystery;
        }
        if (sv == "NonFiction") {
            return Genre::NonFiction;
        }
        if (sv == "SciFi") {
            return Genre::SciFi;
        }
        if (sv == "Biography") {
            return Genre::Biography;
        }
        
        return Genre::Unknown;
    }

    inline auto YearBetween(int from, int to) {
        return [from, to](const Book& b) {
            return b.year >= from && b.year <= to;
        };
    }

    inline auto RatingAbove(double min_rating) {
        return [min_rating](const Book& b) {
            return b.rating > min_rating;
        };
    }

    inline auto GenreIs(Genre genre) {
        return [genre](const Book& b) {
            return b.genre == genre;
        };
    }

    template <typename... Preds>
    auto all_of(Preds... preds) {
        return [=](const Book& b) {
            return (preds(b) && ...);
        };
    }

    template <typename... Preds>
    auto any_of(Preds... preds) {
        return [=](const Book& b) {
            return (preds(b) || ...);
        };
    }
};
}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        std::string genre_str;

        // clang-format off
        using bookdb::Genre;
        switch (g) {
            case Genre::Fiction:    genre_str = "Fiction"; break;
            case Genre::Mystery:    genre_str = "Mystery"; break;
            case Genre::NonFiction: genre_str = "NonFiction"; break;
            case Genre::SciFi:      genre_str = "SciFi"; break;
            case Genre::Biography:  genre_str = "Biography"; break;
            case Genre::Unknown:    genre_str = "Unknown"; break;
            default:
                throw logic_error{"Unsupported bookdb::Genre"};
            }
        // clang-format on
        return format_to(fc.out(), "{}", genre_str);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();
    }
};

}  // namespace std
