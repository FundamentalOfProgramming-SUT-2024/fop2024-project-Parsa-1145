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

    void (*callBack)();
    LinkedList* parentList;

    int shouldClose;

    char* msg;
    int msgLen;
}PopUp;

PopUp* createPopUp(char* message, Widget* parent, int w, int h, voidCB closeCallback, LinkedList* parentList);
void renderPopup(PopUp* popUp);
void deletePopUp(PopUp* popup);
int popupMouseMove(PopUp* p);
int popupMouseClick(PopUp* p);
int isPopupHovered(UiBase* o);
void updatePopup(PopUp* p);
void closePopup(PopUp* p);

#endif