#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

#ifdef USE_GXEPD2
#include <GxEPD2_BW.h>
#else
#include <GxEPD.h>
#include <GxGDEW027W3/GxGDEW027W3.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#endif


#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>

#include "Quotes.h"

class DisplayController {
    public:
        explicit DisplayController();
        void showQuote(Quote* quote);

    private:
        #ifdef USE_GXEPD2
        GxEPD2_BW<GxEPD2_270, GxEPD2_270::HEIGHT> *_display;
        #else
        GxEPD_Class* _display;
        #endif
        char* _splitString(const char* string, const char* separator, int index, bool before=true);

        const GFXfont* _quoteFont;
        const GFXfont* _boldFont;

};

#endif // DISPLAY_CONTROLLER_H