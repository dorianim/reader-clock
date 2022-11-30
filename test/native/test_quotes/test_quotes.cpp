#include <gtest/gtest.h>

#define PROGMEM
#include "../../../src/Quote.cpp"

TEST(Quotes, AllTimes) {
    QuoteList* list = QuoteList::fromAllQuotes();

    for(int hour = 0; hour < 24; hour++) {
        for(int minute = 0; minute < 60; minute++) {
            Quote q = list->findQuoteMatchingTime(hour, minute);
            ASSERT_EQ(q.hour, hour);
            ASSERT_EQ(q.minute, minute);
            ASSERT_NE(q.author, "no quote found");
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
    ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}