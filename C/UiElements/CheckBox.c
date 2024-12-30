#include "Checkbox.h"
#include "Widget.h"

#include "../GlobalDefines.h"
#include "../Globals.h"


void createCheckBox(CheckBox* checkBox, Widget* parent, char* label, int* value, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w){
    checkBox->widget = malloc(sizeof(Widget));
    checkBox->label = label;
    checkBox->value = value;

    createWidget(checkBox->widget, parent, sizePolicyX, sizePolicyY, alignmentX, alignmentY, x, y, w, 1, COLOR_BLACK);

    if(parent!= NULL){
        checkBox->colorPair = parent->bgColor;
    }else{
        checkBox->colorPair = C_BG_BLACK;
    }


}

void renderCheckbox(CheckBox* cb){
    updateWidgetTopLeft(cb->widget);

    if(isWidgetVisible(cb->widget)){
        attron(COLOR_PAIR(cb->colorPair));
        move(cb->widget->topLeftY, cb->widget->topLeftX);
        printw("%s", cb->label);
        
        move(cb->widget->topLeftY, cb->widget->topLeftX + cb->widget->wCopy - 2);

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
void checkBoxMouseMoveCallback(CheckBox* cb){
    updateWidgetTopLeft(cb->widget);

    if((mEvent.y == cb->widget->topLeftY) && (mEvent.x == cb->widget->topLeftX + cb->widget->wCopy - 2)){
        cb->isHovered = 1;
    }else{
        cb->isHovered = 0;
    }
}
void checkBoxMouseClickEvent(CheckBox* cb){
    if(cb->isHovered){
        if(mEvent.bstate & BUTTON1_PRESSED){
            (*cb->value) = !(*cb->value);
        }
    }else{
        cb->isHovered = 0;
    }
}