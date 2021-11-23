#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

#include <GxEPD2_BW.h>

#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>

#include "Quote.h"

class DisplayController {
    public:
        explicit DisplayController();
        void showQuote(Quote* quote);

    private:
        GxEPD2_BW<GxEPD2_270, GxEPD2_270::HEIGHT> *display;
        char* splitString(const char* string, const char* separator, int index, bool before=true);

        const GFXfont* quoteFont;
        const GFXfont* boldFont;

};

#endif // DISPLAY_CONTROLLER_H