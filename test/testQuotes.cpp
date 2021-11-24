#include <unity.h>
#include "../src/Quote.cpp"

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_findQuoteMatchingTime(void) {
    TEST_ASSERT_TRUE(true)
    QuoteList* quotes = new QuoteList(4);
    quotes->append({"Zeit: ", "00:00", " Ende.", "Oscar Wilde", "The Picture of Dorian Gray ", 0, 0, false});
    quotes->append({"Zeit: ", "00:01", " Ende.", "Oscar Wilde", "The Picture of Dorian Gray ", 0, 1, false});
    quotes->append({"Zeit: ", "00:10", " Ende.", "Oscar Wilde", "The Picture of Dorian Gray ", 0, 10, false});
    quotes->append({"Zeit: ", "00:30", " Ende.", "Oscar Wilde", "The Picture of Dorian Gray ", 0, 30, false});

    Quote* q = quotes->findQuoteMatchingTime(0,0,0);
    TEST_ASSERT_FALSE(q == nullptr)
    TEST_ASSERT_EQUAL_STRING("00:00", q->timeText);

    q = quotes->findQuoteMatchingTime(0,1,0);
    TEST_ASSERT_EQUAL_STRING("00:01", q->timeText);

    q = quotes->findQuoteMatchingTime(0,10,0);
    TEST_ASSERT_EQUAL_STRING("00:10", q->timeText);

    q = quotes->findQuoteMatchingTime(0,30,0);
    TEST_ASSERT_EQUAL_STRING("00:30", q->timeText);


    q = quotes->findQuoteMatchingTime(0,2,1);
    TEST_ASSERT_EQUAL_STRING("00:01", q->timeText);

    q = quotes->findQuoteMatchingTime(0,5,4);
    TEST_ASSERT_EQUAL_STRING("00:01", q->timeText);

    q = quotes->findQuoteMatchingTime(0,5,2);
    TEST_ASSERT_EQUAL_STRING("No qoute found, the time is: ", q->textBeforeTime);
    TEST_ASSERT_EQUAL_STRING("00:05", q->timeText);
    TEST_ASSERT_EQUAL_STRING("---", q->author);
}

void process() {
    UNITY_BEGIN();    // IMPORTANT LINE!
    RUN_TEST(test_findQuoteMatchingTime);
    UNITY_END(); // stop unit testing
}
#ifdef ARDUINO
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    
    delay(2000);
    process();
}

void loop() {
}

#else

int main(int argc, char **argv) {
    process();
    return 0;
}

#endif