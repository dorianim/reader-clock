#include "Quote.h"
#include "../Quotes/quotes.h"

QuoteList::QuoteList()
{
    this->huffman_tree = QuoteList::build_tree(quotes_huffman_tree);

    // find indexes of all hours
    Quote current_quote;
    const uint8_t *current_index = quotes_data;
    uint8_t last_hour = -1;
    do {
        const uint8_t *index = current_index;
        this->decode_quote(&current_index, &current_quote);
        if (current_quote.hour != last_hour) {
            this->hour_indexes[current_quote.hour] = index;
            last_hour = current_quote.hour;
        }
    } while (*current_index != 0);
}

QuoteList::~QuoteList()
{
    // TODO: free all HuffmanTree nodes
}

QuoteList *QuoteList::fromAllQuotes()
{
    QuoteList *tmp = new QuoteList();
    return tmp;
}

Quote QuoteList::findQuoteMatchingTime(uint8_t hour, uint8_t minute)
{
    return this->_findQuoteMatchingTime(hour, minute);
}

Quote QuoteList::_findQuoteMatchingTime(uint8_t hour, uint8_t minute)
{
    Quote current_quote;

    int matchingQuoteCount = 0;
    const uint8_t *first_matching_index = nullptr;
    const uint8_t *current_index = this->hour_indexes[hour];

    do
    {
        const uint8_t *index = current_index;
        this->decode_quote(&current_index, &current_quote);

        if (current_quote.hour == hour && current_quote.minute == minute)
        {
            if (first_matching_index == nullptr)
                first_matching_index = index;
            matchingQuoteCount++;
        }
    } while (*current_index != 0 && current_quote.hour == hour);

    if (matchingQuoteCount == 0)
    {
        strcpy(current_quote.textBeforeTime, "It is ");
        sprintf(current_quote.timeText, "%02d:%02d", hour, minute);
        strcpy(current_quote.textAfterTime, "");
        strcpy(current_quote.author, "no quote found");
        strcpy(current_quote.title, "");
        current_quote.hour = hour;
        current_quote.minute = minute;

        return current_quote;
    }

#ifdef ARDUINO
    int chosenQuoteNumber = random(1, matchingQuoteCount);
#else
    int chosenQuoteNumber = 1;
#endif

    for (; chosenQuoteNumber > 0; chosenQuoteNumber--)
    {
        // we assert that the quotes are sorted
        this->decode_quote(&first_matching_index, &current_quote);
    }

    return current_quote;
}

//
// Huffman helpers
//
HuffmanTree *QuoteList::build_tree(const uint8_t *raw_tree)
{
    HuffmanTree *root = nullptr;
    HuffmanTree *last_node = nullptr;

    int children_missing = 0;
    do
    {
        HuffmanTree *current_node = (HuffmanTree *)malloc(sizeof(HuffmanTree));

        current_node->p = last_node;
        current_node->l = nullptr;
        current_node->r = nullptr;
        current_node->c = *raw_tree;
        raw_tree++;

        if (current_node->c == 0)
        {
            children_missing += 2;
            last_node = current_node;
        }

        if (root == nullptr)
        {
            root = current_node;
            continue;
        }

        if (current_node->p->l == nullptr)
        {
            current_node->p->l = current_node;
        }
        else
        {
            current_node->p->r = current_node;
        }
        children_missing--;

        while (last_node != nullptr && last_node->r != nullptr)
        {
            last_node = last_node->p;
        }
    } while (children_missing > 0);

    return root;
}

// decodes encoded into buffer
void QuoteList::decode_quote(const uint8_t **encoded, Quote *buffer)
{
    size_t current_length = 1;
    size_t current_index = 0;
    char *buffers[] = {buffer->textBeforeTime, buffer->timeText, buffer->textAfterTime, buffer->author, buffer->title};
    char **current_buffer = buffers;
    for (;;)
    {
        char c = find_char(this->huffman_tree, *encoded, &current_index);

        if (c == 1)
        {
            (*current_buffer)[current_length - 1] = 0;
            break;
        }
        else if (c == 2)
        {
            (*current_buffer)[current_length - 1] = 0;
            current_buffer++;
            current_length = 0;
        }

        (*current_buffer)[current_length - 1] = c;
        current_length++;
    }
    (*encoded) = *encoded + (current_index / 8 + (current_index % 8 == 0 ? 0 : 1));

    buffer->hour = **encoded;
    (*encoded)++;
    buffer->minute = **encoded;
    (*encoded)++;
}

char QuoteList::find_char(HuffmanTree *tree, const uint8_t encoded[], size_t *current_index)
{
    if (tree->c != 0)
        return tree->c;

    uint8_t current_bit = (encoded[*current_index / 8] >> (7 - (*current_index % 8))) & 1;
    (*current_index)++;

    if (current_bit == 1)
        tree = tree->l;
    else
        tree = tree->r;

    return find_char(tree, encoded, current_index);
}