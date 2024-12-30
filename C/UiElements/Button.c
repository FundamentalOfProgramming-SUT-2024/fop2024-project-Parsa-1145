#include "Button.h"

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "../GlobalDefines.h"


void renderButton(Button* btn){
    if(updateWidgetTopLeft(btn->widget)){
        btn->margin = (btn->widget->wCopy - btn->strLegth) / 2;
        btn->marginCarry = (btn->widget->wCopy - btn->strLegth) % 2;
    }
    
    if(isWidgetVisible(btn->widget->parent)){
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

    btn->str = str;
}
void buttonMouseMoveCallback(Button* btn){
    if(isWidgetVisible(btn->widget->parent)){
        if(isWidgetHovered(btn->widget, mEvent.x, mEvent. y)){
            btn->hovered = 1;
        }else{
            btn->hovered = 0;
            btn->clicked = 0;
        }
    }else{
        btn->hovered = 0;
        btn->clicked = 0;
    }
}
void buttonMouseClickEvent(Button* btn){  //// button is still hovered after clicking
    if(isWidgetVisible(btn->widget->parent)){
        if(btn->hovered){
            if(mEvent.bstate & BUTTON1_PRESSED){
                btn->clicked = 1;
            }else if((mEvent.bstate & BUTTON1_RELEASED) && btn->clicked){
                btn->callBack();
                btn->clicked = 0;
                btn->hovered = 0;
            }else{
                btn->clicked = 0;
            }
        }else{
            btn->clicked = 0;
        }
    }else{
        btn->hovered = 0;
    }
}

void deleteButton(Button* btn){
    deleteWidget(btn->widget);
    free(btn);
}