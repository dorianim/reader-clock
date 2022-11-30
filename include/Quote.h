#ifndef QUOTE_H
#define QUOTE_H

#include "../Quotes/quotes_lengths.h"
#ifdef ARDUINO
#include <Arduino.h>
#else
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#endif

typedef struct Quote
{
    char textBeforeTime[QUOTES_TEXTBEFORE_LENGTH + 1];
    char timeText[QUOTES_TIMETEXT_LENGTH + 1];
    char textAfterTime[QUOTES_TEXTAFTERTIME_LENGTH + 1];
    char author[QUOTES_AUTHOR_LENGTH + 1];
    char title[QUOTES_TITLE_LENGTH + 1];
    uint8_t hour;
    uint8_t minute;
} quote_t;

typedef struct HuffmanTree
{
    char c;
    struct HuffmanTree *p;
    struct HuffmanTree *l;
    struct HuffmanTree *r;
} huffman_tree_t;

class QuoteList
{
public:
    ~QuoteList();

    static QuoteList *fromAllQuotes();

    Quote findQuoteMatchingTime(uint8_t hour, uint8_t minute);

private:
    HuffmanTree *huffman_tree;
    const uint8_t *hour_indexes[23];

    explicit QuoteList();

    Quote _findQuoteMatchingTime(uint8_t hour, uint8_t minute);

    static HuffmanTree *build_tree(const uint8_t *raw_tree);
    static char find_char(HuffmanTree *tree, const uint8_t encoded[], size_t *current_index);
    void decode_quote(const uint8_t **encoded, Quote *buffer);
};

#endif // QUOTE_H