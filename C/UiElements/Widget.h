#ifndef WIDGET2
#define WIDGET2
#define _XOPEN_SOURCE_EXTENDED
#include<ncurses.h>

#include "UiBase.h"
#include"../Utilities/LinkedList.h"



typedef struct Widget{
    struct Widget* parent;
    UiBase* uiBase;
    int x, y;
    int sizeType[2];
    int w, h;
    int alignment[2];
    int isVisible;

    int layoutType;
    int layoutPadding;
    int layoutOffset;
    int outOfBound;
    int scrollOn;
    int canScrollUp;
    int totalScrollArea;
    int scrollBarHeight;
    int scrollBarPos;

    char overflow[2];

    LinkedList* children;

    int bColor;
    int bgColor;

    int scrWCopy, scrHCopy;
    int topLeftX, topLeftY;
    int wCopy, hCopy;

    int colorPair;
    int borderColor;
    int bordered;

    void** tmpIterPtr;
    UiBase* iterPtr;
}Widget;

void createWidget(Widget* widget,Widget* parent, int sizeTypeX, int sizeTypeY, int alignmentX, int alignmentY , int x, int y, int w, int h, int bgColor);
int getWidgetTopLeft(Widget* widget, int* x, int* y);
int getWidgetSize(Widget* widget, int* w, int* h);
int widgetParentResized(Widget* widget);
void renderWidget(Widget* widget);
int updateWidgetSize(Widget* widget);
int updateWidgetTopLeft(Widget* widget);
int isWidgetHovered(Widget* widget, int x, int y);
int isWidgetVisible(Widget* widget);
void updateWidgetChildren(Widget* widget);
void deleteWidget(Widget* widget);
int WMouseMoveCb(Widget* textBox);
int WMouseClickCb(Widget* textBox);
int WKeyPressCb(Widget* textBox, int key);
void emptyWidget(Widget* w);

/*TODO
    first implement vertical layout and scrolling. shouldnt be hard. check if the start of each widget is in the area. dont need to implement a text area for multiple lines

    then add object pointer to buttons and an optional callback which has pointer arguments

    then implement a tab widget. 
    then implement a text area.

    now each item has a respecting button.
*/
#endif