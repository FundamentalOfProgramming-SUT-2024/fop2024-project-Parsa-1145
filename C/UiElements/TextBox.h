#ifndef TEXTBOX
#define TEXTBOX
#include <ncurses.h>

#include "Widget.h"
#include "../GlobalDefines.h"

#define INPUT_TEXT
#define INTPUT_NUMBER


typedef struct{
    Widget* widget;
    UiBase* uiBase;
    int x, y;
    int w, h;
    char* label;

    char* str;
    int* integer;
    int maxCharachters;

    float max, min;

    int type;

    int spaceAccepted;
    int index;

    int sizePolicyX, sizePolicyY;
    int alignmentX, alignmentY;

    voidCB finishedCallback;
    voidCB clickCallback;

    int bgColor;
    int colorPair;

    int focused;
    int hovered;

}TextBox;

void createTextBox(TextBox* textBox, Widget* parent, char* label, char* str, int sizePolicyX, int sizePolicyY, int aliignmentX, int alignmentY, int x, int y, int w);
void createNumberInput(TextBox* textBox, Widget* parent, char* label, int* num, int maxV, int minV, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w);
void renderTextBox(TextBox* textBox);
int TBMouseMoveCb(TextBox* textBox);
int TBMouseClickCb(TextBox* textBox);
int TBKeyPressCb(TextBox* textBox, int key);
void resetTextbox(TextBox* textBox);
void updateTextBox(TextBox* t);
void deleteTextBox(TextBox* t);

#endif