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

template <>
struct formatter<std::pair<bookdb::Genre, double>, char> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const std::pair<bookdb::Genre, double>& p,
                FormatContext& fc) const {
        return format_to(
            fc.out(),
            "{}: {:.2f}",
            p.first,
            p.second
        );
    }
};

template <>
struct formatter<std::vector<std::pair<bookdb::Genre, double>>, char> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(
        const std::vector<std::pair<bookdb::Genre, double>>& vec,
        FormatContext& fc
    ) const {
        auto out = fc.out();

        out = format_to(out, "[");

        bool first = true;
        for (const auto& item : vec) {
            if (!first) {
                out = format_to(out, ", ");
            }
            first = false;
            out = format_to(out, "{}", item);
        }

        out = format_to(out, "]");
        return out;
    }
};

template <>
struct formatter<bookdb::Book> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const bookdb::Book& book, FormatContext& ctx) const {
        return format_to(
            ctx.out(),
            "{} ({}, {}, rating: {}, read: {})",
            book.title,
            book.author,
            book.year,
            book.rating,
            book.read_count
        );
    }
};

}  // namespace std
