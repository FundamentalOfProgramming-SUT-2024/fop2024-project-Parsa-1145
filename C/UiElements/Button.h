#ifndef BUTTON
#define BUTTON
#include <ncurses.h>

#include "../Globals.h"

#include "Widget.h"

typedef struct {
    UiBase* uiBase;
    Widget* widget;

    char* str;
    int strLegth;
    int textAlign;
    int margin;
    int marginCarry;

    void (*callBack)();
    void (*contextCallback)(void*);
    void* contextObject;

    int clicked;

    int bgColor;
}Button;

void renderButton(Button* btn);
void createButton(Button* btn, Widget* parent, char* str, int sizePolicyX, int alignmentX, int alignmentY, int x, int y, int w);
int buttonMouseMoveCallback(Button* btn);
int buttonMouseClickEvent(Button* btn);
void deleteButton(Button* btn);
void updateButton(Button* btn);

#endif