#include "ComboBox.h"

#include <string.h>
#include <stdlib.h>

#include "../Globals.h"
#include "../GameObjects/AudioManager.h"


void createComboBox(ComboBox* c, Widget* parent, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w, int h, int bgColor){
    c->widget = calloc(1, sizeof(Widget));
    c->optionsArea = calloc(1, sizeof(Widget));

    createWidget(c->widget, parent, sizePolicyX, ABSOLUTE, alignmentX, alignmentY, x, y, w, 1, COLOR_BLACK);
    createWidget(c->optionsArea, c->widget, RELATIVE, ABSOLUTE, ALIGN_RIGHT, ABSOLUTE, 1, 0, 100, 1, C_BG_GRAY1);
    c->optionsArea->overflow[1] = 1;
    
    c->options = calloc(1, sizeof(LinkedList));
    createLinkedList(c->options, sizeof(char**));
    c->optionsArea->z = 3;

    c->widget->z = 1;

    c->bgColor = bgColor;

    c->selected = 0;

    c->uiBase = malloc(sizeof(UiBase));
    c->uiBase->render = &comboBoxRender;
    c->uiBase->keyPress = &comboBoxKeyPress;
    c->uiBase->mouseClick = &comboBoxMouseClick;
    c->uiBase->mouseMove = &comboBoxMouseMove;
    c->uiBase->update = &updateComboBox;
    c->uiBase->delete = &deleteComboBox;
    c->uiBase->isHovered = &isComboBoxHovered;
    c->uiBase->z = &(c->widget->z);

    c->uiBase->object = c;
    c->uiBase->widget = c->widget;
    c->uiBase->type = UI_TYPE_COMBOBOX;
}

void comboBoxAddOption(ComboBox* c, char* name){
    linkedListPushBack(c->options, copyString(name));
    c->optionsArea->h += 1;
}
int comboBoxMouseMove(ComboBox* c){
    if(hoveredElement == c->uiBase){
        if(c->optionAreaOpen){
            if(isWidgetHovered(c->optionsArea, mEvent.x, mEvent.y)){
                c->hoveredOption = mEvent.y - c->optionsArea->topLeftY;
                if(c->hoveredOption >= c->options->size){
                    c->hoveredOption = -1;
                }
                return 1;
            }else{
                c->hoveredOption = -1;
            }
        }else{
            if((mEvent.y == c->widget->topLeftY) && (mEvent.x == c->widget->topLeftX + c->widget->wCopy - 1)){
                c->btnHovered = 1;
                return 1;
            }else{
                c->btnHovered = 0;
            }
        }
    }
}
int comboBoxMouseClick(ComboBox* c){
    if(isWidgetVisible(c->widget)){
        if(mEvent.bstate & BUTTON1_PRESSED){
            if((mEvent.y == c->widget->topLeftY) && (mEvent.x == c->widget->topLeftX + c->widget->wCopy - 1)){
                c->btnHovered = 1;
                c->optionAreaOpen = !c->optionAreaOpen;
                playEffectByName("click");
            }else if(c->optionAreaOpen && isWidgetHovered(c->optionsArea, mEvent.x, mEvent.y)){
                if(mEvent.y - c->optionsArea->topLeftY <= c->options->size){
                    c->selected = mEvent.y - c->optionsArea->topLeftY;
                    if(c->optionCallback){
                        c->optionCallback();
                    }
                }
                c->optionAreaOpen = 0;
                c->btnHovered = 0;
                c->hoveredOption = -1;
                playEffectByName("click");
            }else{
                c->optionAreaOpen = 0;
                c->btnHovered = 0;
                c->hoveredOption = -1;
            }
        }
    }
}
int comboBoxKeyPress(ComboBox* c, int ch){

}
void comboBoxRender(ComboBox* c){
    attr_set(0, c->bgColor, NULL);
    moveInFrameBuffer(uiFrameBuffer, c->widget->topLeftY, c->widget->topLeftX);
    FOR(i, c->widget->wCopy - 1){
        addWchToFrameBuffer(uiFrameBuffer, ' ', c->widget->z, c->bgColor, 0);
    }
    if(c->selected != -1){
        mvFramBufferPrintW(uiFrameBuffer, c->widget->topLeftY, c->widget->topLeftX + 1, c->widget->z, "%s", linkedListGetElement(c->options, c->selected));
    }

    attr_set(0, 0, NULL);
    if(c->btnHovered){
        attr_set(WA_REVERSE, c->bgColor, NULL);
    }
    mvAddWchToFrameBuffer(uiFrameBuffer, c->widget->topLeftY, c->widget->topLeftX + c->widget->wCopy - 1, 9662, c->widget->z, 0, 0);
    if(c->btnHovered){
        attr_set(0, c->bgColor, NULL);
    }
    if(c->optionAreaOpen){
        renderWidget(c->optionsArea);
        color_set(c->optionsArea->bgColor, NULL);
        void** tmp = c->options->data;
        char* ptr;
        FOR(i, c->options->size){
            ptr = tmp[1];
            tmp = tmp[0];
            if((i == c->hoveredOption) || (i == c->selected)){
                attr_on(WA_REVERSE, NULL);
            }
            mvFramBufferPrintW(uiFrameBuffer, c->optionsArea->topLeftY + i, c->optionsArea->topLeftX + 1, c->widget->z + 3, "%s", ptr);
            if((i == c->hoveredOption) || (i == c->selected)){
                attr_off(WA_REVERSE, NULL);
            }
        }
    }
    attr_set(0, 0, NULL);
}
void updateComboBox(ComboBox* c){
    updateWidgetTopLeft(c->widget);
    updateWidgetTopLeft(c->optionsArea);
}
void deleteComboBox(ComboBox* c){

}
int isComboBoxHovered(UiBase* o){
    ComboBox* c = o->object;
    if(c->optionAreaOpen){
        if(isWidgetHovered(c->optionsArea, mEvent.x, mEvent.y)){
            if(hoveredZ <= 3){
                hoveredZ = 3;
                hoveredElement = o;
            } 
        }else if(hoveredElement == o){
            hoveredElement = NULL;
            hoveredZ = 0;
        }
    }else{
        if(isWidgetHovered(c->widget, mEvent.x, mEvent.y)){
            if(hoveredZ <= c->widget->z){
                hoveredZ = c->widget->z;
                hoveredElement = o;
            } 
        }else if(hoveredElement == o){
            hoveredElement = NULL;
            hoveredZ = 0;
        }
    }
}