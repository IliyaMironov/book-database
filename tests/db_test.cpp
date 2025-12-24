#include <gtest/gtest.h>
#include <book_database.hpp>
#include <filters.hpp>
#include <statsistics.hpp>
#include <comparators.hpp>

TEST(TestComponentName, SimpleCheck) { EXPECT_EQ(1 + 1, 2); }

TEST(BookDatabaseTest, PushBackIncreasesSize) {
    bookdb::BookDatabase<> db;

    EXPECT_TRUE(db.empty());

    db.PushBack({"Frank Herbert", "Dune", 1965, bookdb::Genre::SciFi, 4.9, 3});

    EXPECT_FALSE(db.empty());
    EXPECT_EQ(db.size(), 1u);
}

TEST(BookDatabaseTest, InitializerListConstructor) {
    bookdb::BookDatabase<> db{
        {"Frank Herbert", "Dune", 1965, bookdb::Genre::SciFi, 4.9, 3},
        {"Agatha Christie", "Poirot", 1934, "Mystery", 4.7, 5},
        {"George Orwell", "1984", 1949, "Fiction", 4.8, 4}
    };

    EXPECT_EQ(db.size(), 3u);
}

TEST(BookDatabaseTest, AuthorsAreCollectedAndUnique) {
    bookdb::BookDatabase<> db;

    db.PushBack({"Frank Herbert", "Dune", 1965, bookdb::Genre::SciFi, 4.9, 3});
    db.PushBack({"Frank Herbert", "Dune Messiah", 1969, bookdb::Genre::SciFi, 4.6, 2});
    db.PushBack({"Agatha Christie", "Poirot", 1934, "Mystery", 4.7, 5});

    const auto& authors = db.GetAuthors();

    EXPECT_EQ(authors.size(), 2u);
    EXPECT_TRUE(authors.contains("Frank Herbert"));
    EXPECT_TRUE(authors.contains("Agatha Christie"));
}

TEST(BookDatabaseTest, IterationWorksLikeStandardContainer) {
    bookdb::BookDatabase<> db{
        {"Frank Herbert", "Dune", 1965, bookdb::Genre::SciFi, 4.9, 3},
        {"Agatha Christie", "Poirot", 1934, "Mystery", 4.7, 5}
    };

    std::size_t count = 0;
    for (const bookdb::Book& book : db) {
        EXPECT_FALSE(book.title.empty());
        ++count;
    }

    EXPECT_EQ(count, db.size());
}

TEST(BookDatabaseTest, EmplaceBackReturnsReference) {
    bookdb::BookDatabase<> db;

    auto& book = db.EmplaceBack(
        "George Orwell",
        "1984",
        1949,
        "Fiction",
        4.8,
        4
    );

    EXPECT_EQ(book.title, "1984");
    EXPECT_EQ(db.size(), 1u);
}

// -------

TEST(BookDatabaseTest, PushBackAndSize) {
    bookdb::BookDatabase<> db;

    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0u);

    db.PushBack({"Frank Herbert", "Dune", 1965, bookdb::Genre::SciFi, 4.9, 3});
    db.PushBack({"Agatha Christie", "Poirot", 1934, "Mystery", 4.7, 5});

    EXPECT_FALSE(db.empty());
    EXPECT_EQ(db.size(), 2u);

    const auto& authors = db.GetAuthors();
    EXPECT_TRUE(authors.contains("Frank Herbert"));
    EXPECT_TRUE(authors.contains("Agatha Christie"));
}

TEST(BookFiltersTest, YearAndRatingAndGenre) {
    bookdb::BookDatabase<> db{
        {"Frank Herbert", "Dune", 1965, bookdb::Genre::SciFi, 4.9, 3},
        {"George Orwell", "1984", 1949, "Fiction", 4.8, 4},
        {"Agatha Christie", "Poirot", 1934, "Mystery", 4.7, 5}
    };

    auto filtered1 = bookdb::filterBooks(db.begin(), db.end(),
                                         bookdb::YearBetween(1935, 1970));
    EXPECT_EQ(filtered1.size(), 2u);

    auto filtered2 = bookdb::filterBooks(db.begin(), db.end(),
                                         bookdb::RatingAbove(4.85));
    EXPECT_EQ(filtered2.size(), 1u);
    EXPECT_EQ(filtered2[0].get().title, "Dune");

    auto combined = bookdb::filterBooks(db.begin(), db.end(),
                                        bookdb::all_of(
                                            bookdb::YearBetween(1900, 1970),
                                            bookdb::RatingAbove(4.7)
                                        ));
    EXPECT_EQ(combined.size(), 2u);

    auto any_filtered = bookdb::filterBooks(db.begin(), db.end(),
                                            bookdb::any_of(
                                                bookdb::GenreIs(bookdb::Genre::Mystery),
                                                bookdb::RatingAbove(4.85)
                                            ));
    EXPECT_EQ(any_filtered.size(), 2u);
}

TEST(BookAnalysisTest, AuthorHistogramAndAverageRating) {
    bookdb::BookDatabase<> db{
        {"Frank Herbert", "Dune", 1965, bookdb::Genre::SciFi, 4.9, 3},
        {"Frank Herbert", "Dune Messiah", 1969, bookdb::Genre::SciFi, 4.6, 2},
        {"Agatha Christie", "Poirot", 1934, "Mystery", 4.7, 5}
    };

    auto hist = bookdb::buildAuthorHistogramFlat(db);
    EXPECT_EQ(hist.size(), 2u);
    EXPECT_EQ(hist[0].first, "Agatha Christie");
    EXPECT_EQ(hist[1].first, "Frank Herbert");
    EXPECT_EQ(hist[1].second, 2u);

    double avg = bookdb::calculateAverageRating(db);
    EXPECT_NEAR(avg, (4.9 + 4.6 + 4.7) / 3.0, 1e-6);

    auto genre_ratings = bookdb::calculateGenreRatings(db);
    for (auto [g, r] : genre_ratings) {
        if (g == bookdb::Genre::SciFi) {
            EXPECT_NEAR(r, (4.9 + 4.6) / 2.0, 1e-6);
        }
    }
}

TEST(BookAnalysisTest, RandomSampleAndTopN) {
    bookdb::BookDatabase<> db{
        {"A", "Book1", 2000, "Fiction", 4.0, 1},
        {"B", "Book2", 2001, "Fiction", 4.5, 2},
        {"C", "Book3", 2002, "SciFi", 4.8, 3},
        {"D", "Book4", 2003, "Mystery", 4.1, 4}
    };

    auto sample = bookdb::sampleRandomBooks(db, 2);
    EXPECT_EQ(sample.size(), 2u);

    auto top = bookdb::getTopNBy(db, 2, bookdb::comp::GreaterByRating{});
    EXPECT_EQ(top.size(), 2u);
    EXPECT_GE(top[0].get().rating, top[1].get().rating);
}

TEST(BookDatabaseTest, EmptyDatabaseHandling) {
    bookdb::BookDatabase<> db;

    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0u);

    EXPECT_THROW(bookdb::calculateAverageRating(db), std::logic_error);
    EXPECT_THROW(bookdb::sampleRandomBooks(db, 1), std::out_of_range);
    EXPECT_THROW(bookdb::getTopNBy(db, 1, bookdb::comp::LessByRating{}), std::out_of_range);
}

TEST(BookFilterTest, CombinedPredicatesWorkCorrectly) {
    bookdb::BookDatabase<> db{
        {"Frank Herbert", "Dune", 1965, bookdb::Genre::SciFi, 4.9, 3},
        {"Frank Herbert", "Dune Messiah", 1969, bookdb::Genre::SciFi, 4.6, 2},
        {"George Orwell", "1984", 1949, bookdb::Genre::Fiction, 4.8, 4},
        {"Agatha Christie", "Poirot", 1934, bookdb::Genre::Mystery, 4.7, 5},
        {"Unknown Author", "Some Book", 2000, bookdb::Genre::Unknown, 3.5, 1}
    };

    // SciFi книги с рейтингом > 4.7, изданные между 1950 и 1970
    auto result = bookdb::filterBooks(
        db.begin(),
        db.end(),
        bookdb::all_of(
            bookdb::GenreIs(bookdb::Genre::SciFi),
            bookdb::RatingAbove(4.7),
            bookdb::YearBetween(1950, 1970)
        )
    );

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result.front().get().title, "Dune");

    // книги либо жанра Mystery, либо с рейтингом > 4.85
    auto any_result = bookdb::filterBooks(
        db.begin(),
        db.end(),
        bookdb::any_of(
            bookdb::GenreIs(bookdb::Genre::Mystery),
            bookdb::RatingAbove(4.85)
        )
    );

    ASSERT_EQ(any_result.size(), 2u);

    std::vector<std::string_view> titles;
    for (const auto& ref : any_result) {
        const auto& b = ref.get();
        titles.push_back(b.title);
    }

    EXPECT_NE(std::find(titles.begin(), titles.end(), "Dune"), titles.end());
    EXPECT_NE(std::find(titles.begin(), titles.end(), "Poirot"), titles.end());
}
