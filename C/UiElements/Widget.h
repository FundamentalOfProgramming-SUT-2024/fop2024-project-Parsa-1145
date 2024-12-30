#ifndef WIDGET2
#define WIDGET2
#include<ncurses.h>



struct Widget{
    struct Widget* parent;
    WINDOW* window;
    int x, y;
    int sizeType[2];
    int w, h;
    int alignment[2];
    int isVisible;

    int bColor;
    int bgColor;

    int scrWCopy, scrHCopy;
    int topLeftX, topLeftY;
    int wCopy, hCopy;

    int colorPair;
};

typedef struct Widget Widget;

void createWidget(Widget* widget,Widget* parent, int sizeTypeX, int sizeTypeY, int alignmentX, int alignmentY , int x, int y, int w, int h, int bColor, int bgColor);
int getWidgetTopLeft(Widget* widget, int* x, int* y);
int getWidgetSize(Widget* widget, int* w, int* h);
int widgetParentResized(Widget* widget);
void renderWidget(Widget* widget);
int updateWidgetSize(Widget* widget);
int updateWidgetTopLeft(Widget* widget);
int isWidgetHovered(Widget* widget, int x, int y);
int isWidgetVisible(Widget* widget);

#endif