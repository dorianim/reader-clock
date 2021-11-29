#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

#define USE_GXEPD2
#ifdef USE_GXEPD2
#include <GxEPD2_3C.h>
#else
#include <GxEPD.h>
#include <GxGDEW042Z15/GxGDEW042Z15.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#endif


#include <Fonts/FreeSerifBold18pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>

#include "Quote.h"

class DisplayController {
    public:
        explicit DisplayController();
        void showQuote(Quote* quote);

    private:
        #ifdef USE_GXEPD2
        GxEPD2_3C<GxEPD2_420c_V2, GxEPD2_420c_V2::HEIGHT> *_display;
        #else
        GxEPD_Class* _display;
        #endif

        uint16_t _padding;
        uint16_t _drawAreaX;
        uint16_t _drawAreaY;
        uint16_t _drawAreaWidth;
        uint16_t _drawAreaHeight;

        const GFXfont* _quoteFont;
        const GFXfont* _boldFont;   

        void _printTextWithBreaksAtSpaces(const char* text, uint16_t maximumY, bool wrap = true);
        void _printAuothorAndTitle(const char* author, const char* title, uint16_t *printedHeight);
        uint16_t _getWidthOfString(const char* string, int16_t x=0, int16_t y=0);
        uint16_t _getHeightOfString(const char* string, int16_t x=0, int16_t y=0);

};

#endif // DISPLAY_CONTROLLER_H