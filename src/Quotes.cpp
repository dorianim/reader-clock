#include "Quotes.h"
#include "../Quotes/quoteObject.h"

QuoteList::QuoteList(size_t initialSize) {
    this->_array = (QuoteData*)malloc(initialSize * sizeof(QuoteData));
    this->_used = 0;
    this->_size = initialSize;
}

QuoteList::~QuoteList() {
  free(this->_array);
  this->_array = NULL;
  this->_used = this->_size = 0;
}

void QuoteList::append(QuoteData quote) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  if (this->_used == this->_size) {
    this->_size += sizeof(QuoteData);
    this->_array = (QuoteData*)realloc(this->_array, this->_size * sizeof(QuoteData));
  }
  this->_array[this->_used++] = quote;
}

QuoteData QuoteList::at(int index) {
    if(index < this->_size) {
        return this->_array[index];
    }
    return QuoteData{};
}

int QuoteList::length() {
    return this->_used;
}

QuoteList* QuoteList::fromAllQuotes() {
    QuoteList* tmp = new QuoteList(sizeof(QuoteDatas) / sizeof(QuoteData));
    tmp->_array = QuoteDatas;
    tmp->_used = tmp->_size;
    return tmp;
}

Quote::Quote(QuoteData data): _data{data} {

}

const char* Quote::text() {
    return this->_data.text;
}
const char* Quote::author() {
    return this->_data.author;
}
const char* Quote::title() {
    return this->_data.title;
}

const char* Quote::textBeforeTime() {
  return this->_splitString(this->text(), "|", 0, true);
}
const char* Quote::timeText() {
    return _splitString(this->text(), "|", 0, false);
}
const char* Quote::textAfterTime() {
    return _splitString(this->text(), "|", 1, false);
}


uint8_t Quote::hour() {
    return this->_data.hour;
}
uint8_t Quote::minute() {
    return this->_data.minute;
}
bool Quote::notSafeForWork() {
    return this->_data.notSafeForWork;
}

char* Quote::_splitString(const char* string, const char* separator, int index, bool before)
{
    char* saveptr;
    char* result = strdup("");
    char* tmpString = strdup(string);
    int i = 0;

    if(!before) index++;
    if(string[0] == separator[0]) i++;

    for (; i<= index; i++, tmpString = NULL) {
        result = strtok_r(tmpString, separator, &saveptr);
        if(result == NULL) {
            result = strdup("");
            break;
        }
    }
    
    return result;
}