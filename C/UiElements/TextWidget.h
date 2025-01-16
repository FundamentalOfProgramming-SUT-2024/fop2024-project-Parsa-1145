#ifndef TEXTWIDGET
#define TEXTWIDGET
#include <ncurses.h>
#include <wchar.h>

#include "Widget.h"
#include "../GlobalDefines.h"
typedef union{
    int* d;
    char* s;
    wchar_t* w;
    float* f;
}Value;
typedef struct{
    Value arg;
    char argType;
}Arg;
typedef struct{
    Widget* widget;
    UiBase* uiBase;
    int x, y;
    char* format;
    int strLength;
    int alignmentX, alignmentY;

    int bgColor;
    int colorPair;

    Arg* args;

}TextWidget;

void createTextWidget(TextWidget* t, Widget* parent, int alignmentX, int alignmentY, int x, int y, char* format, ...);
void renderTextWidget(TextWidget* t);
void updateTextWidget(TextWidget* t);
void deleteTextWidget(TextWidget* t);

#endif