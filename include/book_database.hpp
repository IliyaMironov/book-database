#pragma once

#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    using container_type  = BookContainer;
    using value_type      = Book;
    using reference       = value_type&;
    using const_reference = const value_type&;

    using iterator       = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using size_type      = typename container_type::size_type;

    using AuthorContainer =
        std::unordered_set<std::string, bookdb::TransparentStringHash, bookdb::TransparentStringEqual>;


    BookDatabase() = default;

    BookDatabase(std::initializer_list<Book> init) {
        for (const auto& book : init) {
            // Аналогично PushBack
            auto [it, _] = authors_.emplace(book.author);
            PushBack(Book{std::string_view(*it), book.title, book.year, book.genre, book.rating, book.read_count});
        }
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    bool empty() const noexcept {
        return books_.empty();
    }

    size_type size() const noexcept {
        return books_.size();
    }

    iterator begin() noexcept { return books_.begin(); }
    iterator end()   noexcept { return books_.end(); }

    const_iterator begin() const noexcept { return books_.begin(); }
    const_iterator end()   const noexcept { return books_.end(); }

    const_iterator cbegin() const noexcept { return books_.cbegin(); }
    const_iterator cend()   const noexcept { return books_.cend(); }
 
    void PushBack(const Book& book) {
        auto [it, _] = authors_.emplace(book.author);
        Book newBook = book;
        newBook.author = *it;
        books_.push_back(std::move(newBook));
    }

    void PushBack(Book&& book) {
        auto [it, _] = authors_.emplace(book.author);
        book.author = *it;
        books_.push_back(std::move(book));
    }

    template <typename... Args>
    reference EmplaceBack(Args&&... args) {
        Book temp_book(std::forward<Args>(args)...);
        auto [it, _] = authors_.emplace(temp_book.author);
        Book newBook = temp_book;
        newBook.author = *it;
        books_.push_back(std::move(newBook));
        return books_.back();
    }

    const container_type& GetBooks() const noexcept {
        return books_;
    }

    const AuthorContainer& GetAuthors() const noexcept {
        return authors_;
    }


private:
    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }
        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
