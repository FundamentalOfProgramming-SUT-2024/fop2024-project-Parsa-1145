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
    wchar_t sw;
    float* f;
    int sd;
    float sf;
    int sColor[3];
    int* color[3];

}Arg;
typedef struct{
    Widget* widget;
    UiBase* uiBase;
    int x, y;
    char* format;

    int wrap;

    int strLength;
    int alignmentX, alignmentY;

    int bgColor;

    Arg* args;

}TextWidget;

void createTextWidget(TextWidget* t, Widget* parent, int alignmentX, int alignmentY, int x, int y, char* format, ...);
void changeTextWidget(TextWidget* t, char* format, ...);
void renderTextWidget(TextWidget* t);
void updateTextWidget(TextWidget* t);
void deleteTextWidget(TextWidget* t);
int textWidgetGetStrLength(TextWidget* t);

#endif