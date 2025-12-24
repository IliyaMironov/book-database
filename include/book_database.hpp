#pragma once

#include <print>
#include <string>
#include <string_view>
#include <vector>
#include <set>

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
        std::set<std::string, TransparentStringLess>;

    BookDatabase() = default;

    BookDatabase(std::initializer_list<Book> init) {
        for (const auto& book : init) {
            PushBack(book);
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
        books_.push_back(book);
        authors_.emplace(book.author);
    }

    void PushBack(Book&& book) {
        authors_.emplace(book.author);
        books_.push_back(std::move(book));
    }

    template <typename... Args>
    reference EmplaceBack(Args&&... args) {
        auto& ref = books_.emplace_back(std::forward<Args>(args)...);
        authors_.emplace(ref.author);
        return ref;
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
