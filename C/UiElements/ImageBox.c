#include <stdlib.h>
#include "ImageBox.h"


void createImageBox(ImageBox* iB, Widget* parent, CharTexture* t, int color, int alignmentX, int alignmentY, int x, int y){
    iB->widget = calloc(1, sizeof(Widget));

    createWidget(iB->widget, parent, ABSOLUTE, ABSOLUTE, alignmentX, alignmentY, x, y, t->w, t->h, COLOR_BLACK);

    iB->image = t;
    iB->bgColor = color;

    iB->uiBase = calloc(1, sizeof(UiBase));
    iB->uiBase->render = &renderImageBox;
    iB->uiBase->keyPress = &defaultKeyPressCb;
    iB->uiBase->mouseClick = &defaultMouseClickCb;
    iB->uiBase->mouseMove = &defaultMouseMoveCb;
    iB->uiBase->update = &updateImageBox;
    iB->uiBase->delete = &deleteImageBox;
    iB->uiBase->isHovered = &defaultIsHovered;
    iB->uiBase->object = iB;
    iB->uiBase->widget = iB->widget;
    iB->uiBase->type = UI_TYPE_IMAGEBOX;
}
void renderImageBox(ImageBox* iB){
    Widget* p = iB->widget->parent;
    setRenderBoundingBox(p->topLeftX + p->bordered, p->topLeftY + p->bordered, p->topLeftX + p->wCopy - 1 - p->bordered - p->canScrollUp,
                p->topLeftY + p->hCopy - 1 - p->bordered);
    setWrapMode(0);

    color_set(iB->bgColor, NULL);
    FOR(i, iB->widget->hCopy){
        moveInFrameBuffer(uiFrameBuffer, iB->widget->topLeftY + i, iB->widget->topLeftX);
        FOR(j, iB->widget->wCopy){
            addWchToFrameBuffer(uiFrameBuffer, iB->image->data[i][j], iB->widget->z, 0, 0);
        }
    }
    color_set(0, NULL);
    resetRenderBound();
}
void deleteImageBox(ImageBox* iB){
    
}
void updateImageBox(ImageBox* iB){
    updateWidgetTopLeft(iB->widget);
}