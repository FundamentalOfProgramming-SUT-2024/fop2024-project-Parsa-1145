#ifndef COMBOBOX
#define COMBOBOX

#include "Widget.h"

typedef struct{
    Widget* parent;
    Widget* widget;
    UiBase* uiBase;

    Widget* optionsArea;

    int isVisible;
    int optionAreaOpen;
    int btnHovered;

    int selected;
    int hoveredOption;
    LinkedList* options;

    int bgColor;

    int colorPair;
    int borderColor;
    int bordered;
}ComboBox;

void createComboBox(ComboBox* c, Widget* parent, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w, int h, int bgColor);
void comboBoxAddOption(ComboBox* c, char* name);
int comboBoxMouseMove(ComboBox* c);
int comboBoxMouseClick(ComboBox* c);
int comboBoxKeyPress(ComboBox* c, int ch);
void comboBoxRender(ComboBox* c);
void updateComboBox(ComboBox* c);
void deleteComboBox(ComboBox* c);
int isComboBoxHovered(UiBase* c);

#endif