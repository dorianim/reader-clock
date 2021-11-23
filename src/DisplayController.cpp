#include "DisplayController.h"

DisplayController::DisplayController(): quoteFont{&FreeSerif9pt7b}, boldFont{&FreeSerifBold12pt7b} {
    this->display = new GxEPD2_BW<GxEPD2_270, GxEPD2_270::HEIGHT>(GxEPD2_270(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));
    this->display->init();
  this->display->fillScreen(GxEPD_WHITE);
  this->display->display();
}

void DisplayController::showQuote(Quote* quote) {

  uint16_t box_x = 0;
  uint16_t box_y = 0;
  uint16_t box_w = GxEPD2_270::HEIGHT;
  uint16_t box_h = GxEPD2_270::WIDTH;
  uint16_t cursor_y = box_y;

  Serial.println(quote->text);

  String firstPart = splitString(quote->text, "|", 0, true);
  String timePart = splitString(quote->text, "|", 0, false);
  String secondPart = splitString(quote->text, "|", 1, false);

  Serial.println(firstPart);
  Serial.println(timePart);
  Serial.println(secondPart);

  this->display->setRotation(45);
  this->display->setFont(quoteFont);
  this->display->setTextColor(GxEPD_BLACK);

  this->display->fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  this->display->setCursor(box_x, cursor_y+15);
  this->display->print(firstPart);
  this->display->setFont(boldFont);
  this->display->print(timePart);
  this->display->setFont(quoteFont);
  this->display->print(secondPart); 
  //this->display->updateWindow(box_x, box_y, box_w, box_h, true);
  this->display->display();
}

char* DisplayController::splitString(const char* string, const char* separator, int index, bool before)
{
    char* saveptr;
    char* result = strdup("");
    char* tmpString = strdup(string);

    if(!before) index++;

    for (int i = 0; i<= index; i++, tmpString = NULL) {
        result = strtok_r(tmpString, separator, &saveptr);
        if(result == NULL)
            break;
    }
    
    return result;
}