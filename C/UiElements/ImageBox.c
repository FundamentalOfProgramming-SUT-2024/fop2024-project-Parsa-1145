#include <stdlib.h>
#include "ImageBox.h"


void createImageBox(ImageBox* iB, Widget* parent, CharTexture* t, int color, int alignmentX, int alignmentY, int x, int y){
    iB->widget = calloc(1, sizeof(Widget));

    createWidget(iB->widget, parent, ABSOLUTE, ABSOLUTE, alignmentX, alignmentY, x, y, t->w, t->h, COLOR_BLACK);

    iB->image = copyTexture(t);
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
void changeImageBox(ImageBox* i, CharTexture* t){
    //deleteCharTexture(i->image);

    i->image = copyTexture(t);
    i->widget->hCopy = i->image->h;
    i->widget->wCopy = i->image->w;
    i->widget->h = i->image->h;
    i->widget->w = i->image->w;
}
void renderImageBox(ImageBox* iB){
    Widget* p = iB->widget->parent;
    setRenderBoundingBox(p->topLeftX + p->bordered, p->topLeftY + p->bordered, p->topLeftX + p->wCopy - 1 - p->bordered - p->canScrollUp,
                p->topLeftY + p->hCopy - 1 - p->bordered);
    setWrapMode(0);

    if(iB->bgColor != -1){
        color_set(iB->bgColor, NULL);
        FOR(i, iB->widget->hCopy){
            moveInFrameBuffer(uiFrameBuffer, iB->widget->topLeftY + i, iB->widget->topLeftX);
            FOR(j, iB->widget->wCopy){
                addWchToFrameBuffer(uiFrameBuffer, iB->image->data[i][j], iB->widget->z, 0, 0);
            }
        }
        color_set(0, NULL);
    }else{
        FOR(i, iB->widget->hCopy){
            moveInFrameBuffer(uiFrameBuffer, iB->widget->topLeftY + i, iB->widget->topLeftX);
            FOR(j, iB->widget->wCopy){
                color_set(iB->image->color[i][j], NULL);
                addWchToFrameBuffer(uiFrameBuffer, iB->image->data[i][j], iB->widget->z, 0, 0);
            }
        }
    }
    color_set(0, NULL);
    resetRenderBound();
}
void deleteImageBox(ImageBox* iB){
    free(iB->uiBase);
    deleteCharTexture(iB->image);
    free(iB);
}
void updateImageBox(ImageBox* iB){
    updateWidgetTopLeft(iB->widget);
    iB->widget->hCopy = iB->image->h;
    iB->widget->wCopy = iB->image->w;

}