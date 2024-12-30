#ifndef POPUP
#define POPUP
#include<ncurses.h>

#include "Widget.h"
#include "Button.h"

typedef struct{
   Widget* widget;
   Button* close;

    char* msg;
    int msgLen;
}PopUp;

PopUp* createPopUp(char* message, Widget* parent, int w, int h, voidCB closeCallback);
void renderPopup(PopUp* popUp);
void deletePopUp(PopUp* popup);

#endif