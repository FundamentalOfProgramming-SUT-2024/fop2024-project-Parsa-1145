#include "Button.h"

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "../GlobalDefines.h"


void renderButton(Button* btn){
    btn->margin = (btn->widget->wCopy - btn->strLegth) / 2;
    btn->marginCarry = (btn->widget->wCopy - btn->strLegth) % 2;
    
    if(isWidgetVisible(btn->widget)){
        if(!btn->clicked){
            if(btn->hovered){
                attron(A_DIM);
            }
            attron(A_REVERSE);
        }

        move(btn->widget->topLeftY, btn->widget->topLeftX);
        for(int i = 0; i < btn->margin + btn->marginCarry; i++){
            addch(' ');
        }

        printw("%s", btn->str);

        for(int i = 0; i < btn->margin; i++){
            addch(' ');
        }
        
        if(!btn->clicked){
            if(btn->hovered){
                attroff(A_DIM);
            }
            attroff(A_REVERSE);
        }
    }
}
void createButton(Button* btn, Widget* parent, char* str, int sizePolicyX, int alignmentX, int alignmentY, int x, int y, int w){
    btn->widget = malloc(sizeof(Widget));

    createWidget(btn->widget, parent, sizePolicyX, ABSOLUTE, alignmentX, alignmentY, x, y, w, 1, COLOR_BLACK);

    btn->strLegth = strlen(str);
    btn->margin = (btn->widget->wCopy - btn->strLegth) / 2;
    btn->marginCarry = (btn->widget->wCopy - btn->strLegth) % 2;
    btn->clicked = 0;
    btn->hovered = 0;

    btn->str = str;

    btn->uiBase = malloc(sizeof(UiBase));
    btn->uiBase->render = &renderButton;
    btn->uiBase->keyPress = &defaultKeyPressCb;
    btn->uiBase->mouseClick = &buttonMouseClickEvent;
    btn->uiBase->mouseMove = &buttonMouseMoveCallback;
    btn->uiBase->update = &updateButton;
    btn->uiBase->object = btn;
    btn->uiBase->widget = btn->widget;
    btn->uiBase->type = UI_TYPE_BUTTON;
}
int buttonMouseMoveCallback(Button* btn){
    if(isWidgetVisible(btn->widget)){
        if(isWidgetHovered(btn->widget, mEvent.x, mEvent.y)){
            btn->hovered = 1;
            return 1;
        }else{
            btn->hovered = 0;
            btn->clicked = 0;
            return 0;
        }
    }else{
        btn->hovered = 0;
        btn->clicked = 0;
        return 0;
    }
}
int buttonMouseClickEvent(Button* btn){  //// button is still hovered after clicking
    if(isWidgetVisible(btn->widget)){
        if(isWidgetHovered(btn->widget, mEvent.x, mEvent.y)){
            if(mEvent.bstate & BUTTON1_PRESSED){
                btn->clicked = 1;
                return 1;
            }else if((mEvent.bstate & BUTTON1_RELEASED) && btn->clicked){
                if(btn->contextCallback){
                    btn->contextCallback(btn->contextObject);
                }else if(btn->callBack){
                    btn->callBack();
                }
                btn->clicked = 0;
                return 1;
            }else{
                btn->clicked = 0;
                return 0;
            }
        }else{
            btn->hovered = 0;
            btn->clicked = 0;
            return 0;
        }
    }else{
        btn->hovered = 0;
        return 0;
    }
}

void deleteButton(Button* btn){
    deleteWidget(btn->widget);
    free(btn->uiBase);
    free(btn);
}
void updateButton(Button* btn){
    updateWidgetTopLeft(btn->widget);
}