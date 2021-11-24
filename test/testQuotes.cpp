#include <unity.h>
#include "../src/Quotes.cpp"

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_textBeforeTime(void) {
    Quote q = Quote(QuoteData{"1|2|3", "4", "5", 0, 0, false});
    TEST_ASSERT_EQUAL_STRING("1", q.textBeforeTime());

    q = Quote(QuoteData{"111|222|333", "4", "5", 0, 0, false});
    TEST_ASSERT_EQUAL_STRING("111", q.textBeforeTime());

    q = Quote(QuoteData{"|2|3", "4", "5", 0, 0, false});
    TEST_ASSERT_EQUAL_STRING("", q.textBeforeTime());

    q = Quote(QuoteData{"|222|333", "4", "5", 0, 0, false});
    TEST_ASSERT_EQUAL_STRING("", q.textBeforeTime());
}

void test_textAfterTime(void) {
    Quote q = Quote(QuoteData{"1|2|3", "4", "5", 0, 0, false});
    TEST_ASSERT_EQUAL_STRING("3", q.textAfterTime());

    q = Quote(QuoteData{"111|222|333", "4", "5", 0, 0, false});
    TEST_ASSERT_EQUAL_STRING("333", q.textAfterTime());

    q = Quote(QuoteData{"1|2|", "4", "5", 0, 0, false});
    TEST_ASSERT_EQUAL_STRING("", q.textAfterTime());

    q = Quote(QuoteData{"111|222|", "4", "5", 0, 0, false});
    TEST_ASSERT_EQUAL_STRING("", q.textAfterTime());
}

void test_timeText(void) {
    Quote q = Quote(QuoteData{"1|2|3", "4", "5", 0, 0, false});
    TEST_ASSERT_EQUAL_STRING("2", q.timeText());

    q = Quote(QuoteData{"111|222|333", "4", "5", 0, 0, false});
    TEST_ASSERT_EQUAL_STRING("222", q.timeText());

    // The case of an empty time text is not supported!
}

void process() {
    UNITY_BEGIN();    // IMPORTANT LINE!
    RUN_TEST(test_textBeforeTime);
    RUN_TEST(test_textAfterTime);
    RUN_TEST(test_timeText);
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