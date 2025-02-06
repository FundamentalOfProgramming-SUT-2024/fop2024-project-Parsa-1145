#include <stdlib.h>

#include<string.h>
#include "PopUp.h"
#include "../GlobalDefines.h"


PopUp* createPopUp(char* message, Widget* parent, int w, int h, voidCB closeCallback, LinkedList* parentList){
    PopUp* out = calloc(1, sizeof(PopUp));
    out->msg =  calloc(1, strlen(message) + 1);

    strcpy(out->msg, message);
    wrapText(out->msg, w - 3);
    out->msgLen = strlen(out->msg);

    out->widget = calloc(1, sizeof(Widget));
    createWidget(out->widget, parent, ABSOLUTE, ABSOLUTE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, w, h, C_BG_GRAY1);
    out->widget->z = 20;

    out->close = calloc(1, sizeof(Button));
    createButton(out->close, out->widget, "Ok", ABSOLUTE, ALIGN_CENTER, ALIGN_BOTTOM, 0, 2, 4);
    out->close->callBack = closeCallback;
    out->close->contextObject = out;
    out->close->contextCallback =&closePopup;
    out->close->widget->z = 21;

    out->parentList = parentList;
    linkedListPushBack(out->widget->children, out->close->uiBase);

    out->uiBase = malloc(sizeof(UiBase));
    out->uiBase->render = &renderPopup;
    out->uiBase->mouseClick = &(popupMouseClick);
    out->uiBase->mouseMove = &(popupMouseMove);
    out->uiBase->keyPress = &(defaultKeyPressCb);
    out->uiBase->isHovered = &isPopupHovered;
    out->uiBase->update = &updatePopup;

    out->uiBase->object = out;

    linkedListPushBack(parentList, out->uiBase);
    return out;
}
int popupMouseMove(PopUp* p){
    WMouseMoveCb(p->widget);
}
int popupMouseClick(PopUp* p){
    WMouseClickCb(p->widget);
    if(p->shouldClose) deletePopUp(p);
}

void renderPopup(PopUp* popUp){

    renderWidget(popUp->widget);
    renderButton(popUp->close);
    attron(COLOR_PAIR(popUp->widget->bgColor));

    int line = popUp->widget->topLeftY + 2;

    moveInFrameBuffer(uiFrameBuffer, line, popUp->widget->topLeftX + 2);
    FOR(i, popUp->msgLen){
        if(popUp->msg[i] == '\n'){
            line++;
            moveInFrameBuffer(uiFrameBuffer, line, popUp->widget->topLeftX + 2);
        }else{
            addWchToFrameBuffer(uiFrameBuffer, popUp->msg[i], popUp->widget->z, 0, 0);
        }

    }
    attroff(COLOR_PAIR(popUp->widget->bgColor));
}
void deletePopUp(PopUp* popup){
    removeItemFromLinkedList(popup->parentList, popup->uiBase);
    if(hoveredElement->object == popup){
        hoveredElement = NULL;
        hoveredZ = 0;
    }
    free(popup->msg);
    deleteUiBase(popup->uiBase);
    deleteWidget(popup->widget);
    free(popup);
}
void closePopup(PopUp* p){
    p->shouldClose = 1;
}
int isPopupHovered(UiBase* o){
    PopUp* p = o->object;
    widgetHoveredUpdate(p->widget->uiBase);
    defaultIsHovered(p->close->uiBase);
}
void updatePopup(PopUp* p){
    updateWidgetChildren(p->widget);
}