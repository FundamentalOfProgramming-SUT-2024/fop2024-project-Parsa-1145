#include "Button.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "GlobalDefines.h"


void renderButton(Button* btn){
    if(updateWidgetTopLeft(btn->widget)){
        btn->margin = (btn->widget->wCopy - btn->strLegth) / 2;
        btn->marginCarry = (btn->widget->wCopy - btn->strLegth) % 2;
    }
    
    if(btn->clicked){
        attron(COLOR_PAIR(btn->clkColorPair));
    }else{
        if(btn->hovered){
            attron(A_BOLD);
        }
        attron(COLOR_PAIR(btn->colorPair));
    }

    move(btn->widget->topLeftY, btn->widget->topLeftX);
    for(int i = 0; i < btn->margin + btn->marginCarry; i++){
        addch(' ');
    }

    printw("%s", btn->str);

    for(int i = 0; i < btn->margin; i++){
        addch(' ');
    }
    
    attroff(A_BOLD);
    attroff(COLOR_PAIR(1));
}
void createButton(Button* btn, Widget* parent, char* str, int sizePolicyX, int alignmentX, int alignmentY, int x, int y, int w, int bgC, int c, int clkBgC, int clkC){
    btn->widget = malloc(sizeof(Widget));

    createWidget(btn->widget, parent, sizePolicyX, ABSOLUTE, alignmentX, alignmentY, x, y, w, 1, COLOR_BLACK, COLOR_BLACK);

    btn->strLegth = strlen(str);
    btn->margin = (btn->widget->wCopy - btn->strLegth) / 2;
    btn->marginCarry = (btn->widget->wCopy - btn->strLegth) % 2;

    btn->str = str;

    btn->bgColor    = bgC;
    btn->color      = c;
    btn->clkBgColor = clkBgC;
    btn->clkColor   = clkC;

    init_pair(colorPairNum, c, bgC);
    init_pair(colorPairNum+1, clkC, clkBgC);
    btn->colorPair = colorPairNum;
    btn->clkColorPair = colorPairNum + 1;

    colorPairNum += 2;

}
void buttonMouseMoveCallback(Button* btn){
    if(isWidgetHovered(btn->widget, mEvent.x, mEvent. y)){
        btn->hovered = 1;
    }else{
        btn->hovered = 0;
        btn->clicked = 0;
    }
}
void buttonMouseClickEvent(Button* btn, MEVENT e){  //// button is still hovered after clicking
    if(btn->hovered){
        if(e.bstate & BUTTON1_PRESSED){
            btn->clicked = 1;
        }else if((e.bstate & BUTTON1_RELEASED) && btn->clicked){
            btn->callBack();
            btn->clicked = 0;
            btn->hovered = 0;
        }else{
            btn->clicked = 0;
        }
    }else{
        btn->clicked = 0;
    }
}