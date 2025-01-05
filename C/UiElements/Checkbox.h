#ifndef CHECKBOX
#define CHECKBOX
#include "Widget.h"

typedef struct{
    Widget* widget;
    UiBase* uiBase;
    char* label;
    int* value;

    int x, y;
    int w;
    int isHovered;

    int alignment[2];
    int sizePolicy[2];

    int colorPair;

}CheckBox;

void createCheckBox(CheckBox* cb, Widget* parent, char* label, int* value, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w);
void renderCheckBox(CheckBox* cb);
void CBMouseMoveCb(CheckBox* cb);
void CBMouseClickCb(CheckBox* cb);

#endif