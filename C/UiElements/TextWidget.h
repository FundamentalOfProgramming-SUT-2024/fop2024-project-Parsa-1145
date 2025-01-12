#ifndef TEXTWIDGET
#define TEXTWIDGET
#include <ncurses.h>

#include "Widget.h"
#include "../GlobalDefines.h"

typedef struct{
    Widget* widget;
    UiBase* uiBase;
    int x, y;
    char* str;
    int spaceAccepted;
    int index;

    int alignmentX, alignmentY;

    int bgColor;
    int colorPair;

    int focused;
    int hovered;

}TextWidget;

void createTextWidget(TextWidget* t, Widget* parent, char* str, int alignmentX, int alignmentY, int x, int y);
void renderTextWidget(TextWidget* t);
void updateTextWidget(TextWidget* t);
void deleteTextWidget(TextWidget* t);

#endif