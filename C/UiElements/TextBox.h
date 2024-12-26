#ifndef TEXTBOX
#define TEXTBOX
#include <ncurses.h>
#include "Widget.h"
#include "../GlobalDefines.h"

typedef struct{
    Widget* widget;
    Widget* parent;
    int x, y;
    int w, h;
    char* label;
    char* str;
    char* placeHolder;
    int index;

    int sizePolicyX, sizePolicyY;
    int alignmentX, alignmentY;

    voidCB typeCallback;
    voidCB clickCallback;

    int bgColor;
    int colorPair;

    int focused;
    int hovered;

}TextBox;

void createTextBox(TextBox* textBox, Widget* parent, char* label, char* str, int sizePolicyX, int sizePolicyY, int aliignmentX, int alignmentY, int x, int y, int w);
void renderTextBox(TextBox* textBox);
void textBoxMouseMoveCallback(TextBox* textBox);
void textBoxMouseClickCallback(TextBox* textBox);
void textBoxKeyPressCallback(TextBox* textBox, int key);
void resetTextbox(TextBox* textBox);

#endif