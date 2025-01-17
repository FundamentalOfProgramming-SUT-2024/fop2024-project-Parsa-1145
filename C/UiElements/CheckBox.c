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
    checkBox->uiBase->widget = checkBox->widget;
    checkBox->uiBase->object = checkBox;
    checkBox->uiBase->type = UI_TYPE_CHECKBOX;
}

void renderCheckBox(CheckBox* cb){
    if(isWidgetVisible(cb->widget)){
        attron(COLOR_PAIR(cb->colorPair));
        move(cb->widget->topLeftY, cb->widget->topLeftX);
        printw("%s", cb->label);
        
        move(cb->widget->topLeftY, cb->widget->topLeftX + cb->widget->wCopy - 1);

        if(cb->isHovered){
            attron(A_DIM);
        }
        if(*(cb->value)){
            printw("1");
        }else{
            printw("0");
        }
        if(cb->isHovered){
            attroff(A_DIM);
        }
        attroff(COLOR_PAIR(cb->colorPair));

    }
}
void CBMouseMoveCb(CheckBox* cb){

    if((mEvent.y == cb->widget->topLeftY) && (mEvent.x == cb->widget->topLeftX + cb->widget->wCopy - 1)){
        cb->isHovered = 1;
    }else{
        cb->isHovered = 0;
    }
}
void CBMouseClickCb(CheckBox* cb){
    if(cb->isHovered){
        if(mEvent.bstate & BUTTON1_PRESSED){
            (*cb->value) = !(*cb->value);
        }
    }else{
        cb->isHovered = 0;
    }
}
void updateCheckBox(CheckBox* cb){
    updateWidgetTopLeft(cb->widget);
}
void deleteCheckBox(CheckBox* cb){
    free(cb->uiBase);
    free(cb->widget);
    free(cb);
}