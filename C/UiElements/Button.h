#ifndef BUTTON
#define BUTTON
#include <ncurses.h>

#include "../Globals.h"

#include "Widget.h"

typedef struct {
    Widget* parent;
    char* str;
    int strLegth;
    int margin;
    int marginCarry;
    voidCB callBack;
    int hovered;
    int clicked;

    Widget* widget;
}Button;

void renderButton(Button* btn);
//void createButton(Button* btn, int x, int y,int bgC, int c, int clkBgC, int clkC, char* str);
void createButton(Button* btn, Widget* parent, char* str, int sizePolicyX, int alignmentX, int alignmentY, int x, int y, int w);
void buttonMouseMoveCallback(Button* btn);
void buttonMouseClickEvent(Button* btn);
void deleteButton(Button* btn);

#endif