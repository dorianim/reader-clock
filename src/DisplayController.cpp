#include "DisplayController.h"
#define MAX(a,b) (((a)>(b))?(a):(b))

DisplayController::DisplayController(): _quoteFont{&FreeSerif12pt7b}, _boldFont{&FreeSerifBold12pt7b} {
    // GDEW027W3 2.7" b/w
    // https://www.waveshare.com/product/displays/e-paper/2.7inch-e-paper-hat.htm

    #ifdef USE_GXEPD2
    this->_display = new GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT>(GxEPD2_420(/*CS=5*/ SS, /*DC=*/ 15, /*RST=*/ 2, /*BUSY=*/ 4));
    #else
        GxIO_Class* io = new GxIO_Class(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
        this->_display = new GxEPD_Class(*io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4
    #endif

    this->_display->init(115200, true, 2, false);
    this->_display->setTextWrap(false); // wrapping is done manually
    Serial.println("Init done");
    //this->_display->fillScreen(GxEPD_WHITE);
    #ifdef USE_GXEPD2
    //this->_display->display();
    #else
    this->_display->update();
    #endif
    #ifdef ROTATE
    this->_display->setRotation(45);
    #endif
    Serial.println("Clearing done");

    uint16_t displayWidth = this->_display->width();
    uint16_t displayHeight = this->_display->height();

    Serial.printf("Display size: w: %d, h: %d\n", displayWidth, displayHeight);

    this->_padding = float(displayHeight) * 0.05;
    this->_drawAreaX = this->_padding;
    this->_drawAreaY = this->_padding;
    this->_drawAreaHeight = displayHeight - (this->_padding * 2);
    this->_drawAreaWidth = displayWidth - (this->_padding * 2);

    this->_display->setFont(_quoteFont);
}

void DisplayController::showQuote(Quote* quote) {
  this->_display->fillScreen(GxEPD_WHITE);

  // Author and title
  this->_display->setFont(_quoteFont);
  uint16_t bottomBoxPrintedHeight;
  this->_printAuothorAndTitle(quote->author, quote->title, &bottomBoxPrintedHeight);

  // Quote
  this->_display->setCursor(this->_drawAreaX, this->_drawAreaY+15);
  uint16_t maxiumY = (this->_drawAreaY + this->_drawAreaHeight) - bottomBoxPrintedHeight * 2;
  this->_display->setFont(_quoteFont);
  this->_display->setTextColor(GxEPD_BLACK);
  this->_printTextWithBreaksAtSpaces(quote->textBeforeTime, maxiumY);
  this->_display->setFont(_boldFont);
  this->_display->setTextColor(GxEPD_BLACK);
  this->_printTextWithBreaksAtSpaces(quote->timeText, maxiumY);
  this->_display->setTextColor(GxEPD_BLACK);
  this->_display->setFont(_quoteFont);
  this->_printTextWithBreaksAtSpaces(quote->textAfterTime, maxiumY);

  #ifdef USE_GXEPD2
  this->_display->display();
  #else
  this->_display->update();
  #endif
}

void DisplayController::showWarning(const char* message) {
  this->_display->fillScreen(GxEPD_WHITE);

  this->_display->drawBitmap(
    this->_drawAreaX + (this->_drawAreaWidth - exclamationTriangleSolidWidth) / 2, 
    this->_drawAreaY, 
    (uint8_t *)exclamationTriangleSolidBitmap, 
    exclamationTriangleSolidWidth, 
    exclamationTriangleSolidHeight, 
    GxEPD_BLACK
  );

  this->_display->setFont(_boldFont);
  this->_display->setTextColor(GxEPD_BLACK);
  this->_display->setCursor(this->_drawAreaX, this->_drawAreaY + 2 * exclamationTriangleSolidHeight + this->_padding);
  this->_printTextWithBreaksAtSpaces(message, this->_drawAreaY + this->_drawAreaHeight);

  this->_display->display();
}

void DisplayController::_printTextWithBreaksAtSpaces(const char* text, uint16_t maximumY, bool wrap) {

  char* result = strdup("");
  char* tmpString = strdup(text);
  int16_t previousY = this->_display->getCursorY();
  int16_t previousX = this->_display->getCursorX();
  uint16_t previousHeight = 0;
  uint16_t previousWidth = 0;

  if(this->_display->getCursorY() >= maximumY)
    return;

  int textLength = strlen(text);
  if(textLength > 0 && text[0] == ' ')
    this->_display->print(" ");

  while (1) {
    result = strtok(tmpString, " ");
    delete tmpString;
    if(result == NULL) break;

    if(tmpString == NULL)
      this->_display->print(" ");
    tmpString = NULL;

    int16_t currentX = this->_display->getCursorX();
    int16_t currentY = this->_display->getCursorY();
    uint16_t w = this->_getWidthOfString(result, currentX, currentY);
    
    if(currentX + w > this->_drawAreaWidth) {
      if(!wrap) break;
      this->_display->println();
      this->_display->setCursor(this->_drawAreaX, this->_display->getCursorY());
    }

    if(this->_display->getCursorY() >= maximumY) {
      // replace the last word of the previous row with "..."
      this->_display->setFont(_quoteFont);
      this->_display->fillRect(previousX-previousWidth, previousY-previousHeight, previousWidth+2, previousHeight*1.5, GxEPD_WHITE);
      this->_display->setCursor(previousX-previousWidth, previousY);
      this->_display->println("...");
      break;
    }

    this->_display->print(result);
    previousY = this->_display->getCursorY();
    previousX = this->_display->getCursorX();
    previousHeight = this->_getHeightOfString(result);
    previousWidth = w;
  }

  if(textLength > 0 && text[textLength-1] == ' ')
    this->_display->print(" ");

}

void DisplayController::_printAuothorAndTitle(const char* author, const char* title, uint16_t *printedHeight) {
  const char* strings[] = {author, title};
  this->_display->setTextColor(GxEPD_BLACK);

  *printedHeight = 0;
  uint16_t previousY = 0;
  // loop over both strings and print them from bottom to top
  for(int i=0; i < 2; i++) {
    char* result = strdup("");
    char* tmpString = strdup(strings[i]);
    String trimmedString = "";
    bool wasTrimmed = false;

    // loop over all words and check if they fit
    while (1) {
      result = strtok(tmpString, " ");
      tmpString = NULL;
      if(result == NULL) break;

      String tempTestString = trimmedString + " " + String(result);
      uint16_t w = this->_getWidthOfString(tempTestString.c_str());

      if(w > this->_drawAreaWidth){
        wasTrimmed = true;
        break;
      }
      if(trimmedString.length() > 0) trimmedString += " ";
      trimmedString += String(result);
    }

    for(int j=1; j < 4 && wasTrimmed; j++) {
      trimmedString[trimmedString.length()-j] = '.';
    }

    uint16_t width = this->_getWidthOfString(trimmedString.c_str());
    uint16_t height = this->_getHeightOfString(trimmedString.c_str());
    *printedHeight += height;
    int16_t x = MAX((this->_drawAreaX + this->_drawAreaWidth) - width, this->_drawAreaX);
    uint16_t y = 0;
    if(i == 0)
      y = this->_drawAreaY + this->_drawAreaHeight - height;
    else
      y = previousY - height - this->_padding * 0.5;
    
    previousY = y;

    this->_display->fillRect(this->_drawAreaX, y - this->_padding, this->_drawAreaWidth + 2, height + this->_padding + 2, GxEPD_WHITE);
    this->_display->setCursor(x, y + height * 0.75);
    this->_display->print(trimmedString);
  }
}

uint16_t DisplayController::_getWidthOfString(const char* string, int16_t x, int16_t y) {
  int16_t x1, y1;
  uint16_t w, h;
  this->_display->getTextBounds(string, x, y, &x1, &y1, &w, &h);
  return w;
}

uint16_t DisplayController::_getHeightOfString(const char* string, int16_t x, int16_t y) {
  int16_t x1, y1;
  uint16_t w, h;
  this->_display->getTextBounds(string, x, y, &x1, &y1, &w, &h);
  return h;
}