#include <stdlib.h>

#include<string.h>
#include "PopUp.h"
#include "../GlobalDefines.h"


PopUp* createPopUp(char* message, Widget* parent, int w, int h, voidCB closeCallback){
    PopUp* out = malloc(sizeof(PopUp));
    out->msg =  malloc(strlen(message) + 1);
    strcpy(out->msg, message);
    wrapText(out->msg, w - 2);
    out->msgLen = strlen(out->msg);

    out->widget = malloc(sizeof(Widget));

    createWidget(out->widget, parent, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, w, h, C_BG_GRAY1);

    out->close = malloc(sizeof(Button));
    createButton(out->close, out->widget, "Ok", ABSOLUTE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 2, 4);
    out->close->callBack = closeCallback;

    out->uiBase = malloc(sizeof(UiBase));
    out->uiBase->render = &renderPopup;
    out->uiBase->mouseClick = &(popupMouseClick);
    out->uiBase->mouseMove = &(popupMouseClick);
    out->uiBase->keyPress = &(defaultKeyPressCb);
    out->uiBase->object = out;

    return out;
}
int popupMouseMove(PopUp* p){
    return p->close->uiBase->mouseMove(p->close);
}
int popupMouseClick(PopUp* p){
    return p->close->uiBase->mouseClick(p->close);
}


void renderPopup(PopUp* popUp){
    renderWidget(popUp->widget);
    renderButton(popUp->close);
    attron(COLOR_PAIR(popUp->widget->bgColor));

    int line = popUp->widget->topLeftY + 2;

    move(line, popUp->widget->topLeftX + 2);
    FOR(i, popUp->msgLen){
        if(popUp->msg[i] == '\n'){
            line++;
            move(line, popUp->widget->topLeftX + 2);
        }else{
            addch(popUp->msg[i]);
        }

    }
    attroff(COLOR_PAIR(popUp->widget->bgColor));
}
void deletePopUp(PopUp* popup){
    free(popup->msg);
    free(popup->uiBase);
    deleteButton(popup->close);
    deleteWidget(popup->widget);
    free(popup);
}