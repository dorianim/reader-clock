#include "DisplayController.h"

DisplayController::DisplayController(): _quoteFont{&FreeSerif9pt7b}, _boldFont{&FreeSerifBold12pt7b} {
    // GDEW027W3 2.7" b/w
    // https://www.waveshare.com/product/displays/e-paper/2.7inch-e-paper-hat.htm

    #ifdef USE_GXEPD2
    this->_display = new GxEPD2_BW<GxEPD2_270, GxEPD2_270::HEIGHT>(GxEPD2_270(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));
    #else
        GxIO_Class* io = new GxIO_Class(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
        this->_display = new GxEPD_Class(*io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4
    #endif
    this->_display->init();
    Serial.println("Init done");
    this->_display->fillScreen(GxEPD_WHITE);
    #ifdef USE_GXEPD2
    this->_display->display();
    #else
    this->_display->update();
    #endif
    Serial.println("Clearing done");
}

void DisplayController::showQuote(Quote* quote) {

  uint16_t box_x = 0;
  uint16_t box_y = 0;
  #ifdef USE_GXEPD2
  uint16_t box_w = GxEPD2_270::HEIGHT;
  uint16_t box_h = GxEPD2_270::WIDTH;
  #else
  uint16_t box_w = this->_display->width();
  uint16_t box_h = this->_display->height();
  #endif
  uint16_t cursor_y = box_y;

  this->_display->setRotation(45);
  this->_display->setFont(_quoteFont);
  this->_display->setTextColor(GxEPD_BLACK);

  this->_display->fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  this->_display->setCursor(box_x, cursor_y+15);
  this->_display->print(quote->textBeforeTime);
  this->_display->setFont(_boldFont);
  this->_display->setTextColor(GxEPD_BLACK);
  this->_display->print(quote->timeText);
  this->_display->setTextColor(GxEPD_BLACK);
  this->_display->setFont(_quoteFont);
  this->_display->print(quote->textAfterTime); 
  //this->display->updateWindow(box_x, box_y, box_w, box_h, true);
  #ifdef USE_GXEPD2
  this->_display->display();
  #else
  this->_display->update();
  #endif
}