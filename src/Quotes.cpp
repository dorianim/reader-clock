#include "Quotes.h"
#include "../Quotes/quoteObject.h"

QuoteList::QuoteList(size_t initialSize) {
    this->_array = (Quote*)malloc(initialSize * sizeof(Quote));
    this->_used = 0;
    this->_size = initialSize;
}

QuoteList::~QuoteList() {
  free(this->_array);
  this->_array = NULL;
  this->_used = this->_size = 0;
}

void QuoteList::append(Quote quote) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  if (this->_used == this->_size) {
    this->_size += sizeof(Quote);
    this->_array = (Quote*)realloc(this->_array, this->_size * sizeof(Quote));
  }
  this->_array[this->_used++] = quote;
}

Quote QuoteList::at(int index) {
    if(index < this->_size) {
        return this->_array[index];
    }
    return Quote{};
}

int QuoteList::length() {
    return this->_used;
}

QuoteList* QuoteList::fromAllQuotes() {
    QuoteList* tmp = new QuoteList(sizeof(quotes) / sizeof(Quote));
    tmp->_array = quotes;
    tmp->_used = tmp->_size;
    return tmp;
}