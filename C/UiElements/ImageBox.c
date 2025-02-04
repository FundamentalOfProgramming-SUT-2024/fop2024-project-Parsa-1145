#include <stdlib.h>
#include "ImageBox.h"


void createImageBox(ImageBox* iB, Widget* parent, CharTexture* t, int alignmentX, int alignmentY, int x, int y){
    iB->widget = calloc(1, sizeof(ImageBox));

    createWidget(iB->widget, parent, ABSOLUTE, ABSOLUTE, alignmentX, alignmentY, x, y, t->w, t->h, COLOR_BLACK);

    iB->image = t;

    iB->uiBase = calloc(1, sizeof(UiBase));
    iB->uiBase->render = &renderImageBox;
    iB->uiBase->keyPress = &defaultKeyPressCb;
    iB->uiBase->mouseClick = &defaultMouseClickCb;
    iB->uiBase->mouseMove = &defaultMouseMoveCb;
    iB->uiBase->update = &updateImageBox;
    iB->uiBase->delete = &deleteImageBox;
    iB->uiBase->object = iB;
    iB->uiBase->widget = iB->widget;
    iB->uiBase->type = UI_TYPE_BUTTON;

}
void renderImageBox(ImageBox* iB){
    
}
void deleteImageBox(ImageBox* iB){
    
}
void updateImageBox(ImageBox* iB){
    updateWidgetTopLeft(iB->widget);
}