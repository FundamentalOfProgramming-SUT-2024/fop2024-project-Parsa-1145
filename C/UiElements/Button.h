#ifndef BUTTON
#define BUTTON
#include <ncurses.h>

#include "../Globals.h"

#include "Widget.h"

typedef struct {
    Widget* parent;
    UiBase* uiBase;
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
void createButton(Button* btn, Widget* parent, char* str, int sizePolicyX, int alignmentX, int alignmentY, int x, int y, int w);
int buttonMouseMoveCallback(Button* btn);
int buttonMouseClickEvent(Button* btn);
void deleteButton(Button* btn);

#endif