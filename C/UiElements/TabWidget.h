#ifndef TABWIDGET
#define TABWIDGET
#include "UiBase.h"
#include "Button.h"
#include "Widget.h"
typedef struct{
    Widget* parent;
    Widget* widget;
    UiBase* uiBase;
    int isVisible;

    LinkedList* Tabs;
    LinkedList* tabButtons;
    Widget* active;
    Widget* tabArea;

    int bgColor;

    int colorPair;
    int borderColor;
    int bordered;

    void** tmpIterPtr;
    UiBase* iterPtr;
    Widget* tmpWidget;
}TabWidget;

typedef struct{
    TabWidget* tabWidget;
    Widget* tab;
    void (*enterCallback)();
}TabStruct;

void createTabWidget(TabWidget* t, Widget* parent, int sizePolicyX, int sizePolicyY, int alignmentLeft, int alignmentRight, int x, int y, int w, int h, int bgColor);
void tabWidgetAddTab(TabWidget* t, char* name, Widget* w, void(*callback)());
int tabWidgetMouseMove(TabWidget* t);
int tabWidgetMouseClick(TabWidget* t);
int tabWidgetKeyPress(TabWidget* t, int ch);
int tabWidgetScroll(TabWidget* t);
void tabWidgetRender(TabWidget* t);
void tabWidgetSwitchTab(TabStruct* t);
void updateTabWidget(TabWidget* t);

#endif