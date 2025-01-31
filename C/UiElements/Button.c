#include "Button.h"

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "../GlobalDefines.h"


void renderButton(Button* btn){
    if(isWidgetVisible(btn->widget)){
        attron(COLOR_PAIR(btn->bgColor));
        if(btn->clicked){
            attron(A_REVERSE);
        }else if(btn->hovered){
            attron(A_BOLD);
        }
        move(btn->widget->topLeftY, btn->widget->topLeftX);
        for(int i = 0; i < btn->widget->wCopy; i++){
            addch(' ');
        }
        switch (btn->textAlign){
            case ALIGN_CENTER:
                btn->margin = (btn->widget->wCopy - btn->strLegth)/2 + ((btn->widget->wCopy - btn->strLegth)&1);
                break;
            case ALIGN_LEFT:
                btn->margin = 0;
                break;
            default:
                break;
        }
        mvprintw(btn->widget->topLeftY, btn->widget->topLeftX + btn->margin,"%s", btn->str);
        if(btn->clicked){
            attroff(A_REVERSE);
        }else if(btn->hovered){
            attroff(A_BOLD);
        }
        attroff(COLOR_PAIR(btn->bgColor));
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

    btn->str = copyString(str);
    btn->textAlign = ALIGN_CENTER;
    btn->bgColor = C_BG_WHITE;
    btn->contextCallback = 0;
    btn->callBack = 0;

    btn->uiBase = malloc(sizeof(UiBase));
    btn->uiBase->render = &renderButton;
    btn->uiBase->keyPress = &defaultKeyPressCb;
    btn->uiBase->mouseClick = &buttonMouseClickEvent;
    btn->uiBase->mouseMove = &buttonMouseMoveCallback;
    btn->uiBase->update = &updateButton;
    btn->uiBase->delete = &deleteButton;
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
    free(btn->str);
    free(btn);
}
void updateButton(Button* btn){
    updateWidgetTopLeft(btn->widget);
}