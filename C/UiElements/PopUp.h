#ifndef POPUP
#define POPUP
#include<ncurses.h>

#include "Widget.h"
#include "Button.h"
#include "../Utilities/LinkedList.h"

typedef struct{
    Widget* widget;
    Button* close;
    UiBase* uiBase;

    char* msg;
    int msgLen;
}PopUp;

PopUp* createPopUp(char* message, Widget* parent, int w, int h, voidCB closeCallback);
void renderPopup(PopUp* popUp);
void deletePopUp(PopUp* popup);
int popupMouseMove(PopUp* p);
int popupMouseClick(PopUp* p);


#endif