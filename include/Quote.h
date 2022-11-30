#ifndef QUOTE_H
#define QUOTE_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#endif

typedef struct Quote
{
    char textBeforeTime[200];
    char timeText[50];
    char textAfterTime[500];
    char author[50];
    char title[50];
    uint8_t hour;
    uint8_t minute;
    bool notSafeForWork;
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
    HuffmanTree* huffman_tree;

    explicit QuoteList();

    Quote _findQuoteMatchingTime(uint8_t hour, uint8_t minute);

    static HuffmanTree *build_tree(const uint8_t *raw_tree);
    static char find_char(HuffmanTree* tree, const uint8_t encoded[], size_t *current_index);
    void decode_quote(const uint8_t **encoded, Quote *buffer);
};

#endif // QUOTE_H