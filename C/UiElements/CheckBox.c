#include <stdlib.h>

#include "Checkbox.h"
#include "Widget.h"

#include "../GlobalDefines.h"
#include "../Globals.h"


void createCheckBox(CheckBox* checkBox, Widget* parent, char* label, int* value, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w){
    checkBox->widget = malloc(sizeof(Widget));
    checkBox->label = label;
    checkBox->value = value;

    createWidget(checkBox->widget, parent, sizePolicyX, sizePolicyY, alignmentX, alignmentY, x, y, w, 1, COLOR_BLACK);

    if((parent) && (parent->bgColor)){
        checkBox->colorPair = parent->bgColor;
    }else{
        checkBox->colorPair = C_BG_BLACK;
    }

    checkBox->uiBase = malloc(sizeof(UiBase));
    checkBox->uiBase->render = &renderCheckBox;
    checkBox->uiBase->keyPress = &defaultKeyPressCb;
    checkBox->uiBase->mouseClick = &CBMouseClickCb;
    checkBox->uiBase->mouseMove = &CBMouseMoveCb;
    checkBox->uiBase->update = &updateCheckBox;
    checkBox->uiBase->delete = &deleteCheckBox;
    checkBox->uiBase->isHovered = &defaultIsHovered;
    checkBox->uiBase->z = &(checkBox->widget->z);

    checkBox->uiBase->widget = checkBox->widget;
    checkBox->uiBase->object = checkBox;
    checkBox->uiBase->type = UI_TYPE_CHECKBOX;
}

void renderCheckBox(CheckBox* cb){
    if(isWidgetVisible(cb->widget)){
        attron(COLOR_PAIR(cb->colorPair));
        moveInFrameBuffer(uiFrameBuffer ,cb->widget->topLeftY, cb->widget->topLeftX);
        framBufferPrintW(uiFrameBuffer, cb->widget->z,"%s", cb->label);
        
        moveInFrameBuffer(uiFrameBuffer ,cb->widget->topLeftY, cb->widget->topLeftX + cb->widget->wCopy - 1);

        if(hoveredElement == cb->uiBase){
            attron(A_DIM);
        }
        if(*(cb->value)){
            addWchToFrameBuffer(uiFrameBuffer, '1', cb->widget->z, cb->widget->bgColor, 0);
        }else{
            addWchToFrameBuffer(uiFrameBuffer, '0', cb->widget->z, cb->widget->bgColor, 0);
        }
        if(hoveredElement == cb->uiBase){
            attroff(A_DIM);
        }
        attroff(COLOR_PAIR(cb->colorPair));
    }
}
void CBMouseMoveCb(CheckBox* cb){
}
void CBMouseClickCb(CheckBox* cb){
    if(hoveredElement == cb->uiBase){
        if(mEvent.bstate & BUTTON1_PRESSED){
            (*cb->value) = !(*cb->value);
        }
    }
}
void updateCheckBox(CheckBox* cb){
    updateWidgetTopLeft(cb->widget);
}
void deleteCheckBox(CheckBox* cb){
    deleteUiBase(cb->uiBase);
    free(cb->widget);
    free(cb);
}
int isCheckboxHovered(UiBase* o){
    CheckBox* cb = o->object;
    if((mEvent.y == cb->widget->topLeftY) && (mEvent.x == cb->widget->topLeftX + cb->widget->wCopy - 1)){
        if(o->z >= hoveredZ){
            hoveredZ = o->z;
            hoveredElement = o;
        }
    }else if(hoveredElement == o){
        hoveredElement = NULL;
        hoveredZ = 0;
    }
}