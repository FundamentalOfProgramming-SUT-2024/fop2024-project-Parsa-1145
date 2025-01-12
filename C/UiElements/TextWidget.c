#include "TextWidget.h"

#include <string.h>
#include <stdlib.h>

#include "../Globals.h"

void createTextWidget(TextWidget* t, Widget* parent, char* str, int alignmentX, int alignmentY, int x, int y){
    t->widget = malloc(sizeof(Widget));

    createWidget(t->widget, parent, ABSOLUTE, ABSOLUTE, alignmentX, alignmentY, x, y, 1, 4, NULL);
    
    t->str = str;

    if((parent) && (parent->bgColor)){
        t->bgColor = parent->bgColor;
    }else{
        t->bgColor = COLOR_BLACK;
    }

    t->spaceAccepted = 0;
    t->focused = 0;

    t->uiBase = malloc(sizeof(UiBase));
    t->uiBase->render = &renderTextWidget;
    t->uiBase->keyPress = &defaultKeyPressCb;
    t->uiBase->mouseClick = &defaultMouseClickCb;
    t->uiBase->mouseMove = &defaultMouseMoveCb;
    t->uiBase->update = &updateTextWidget;
    t->uiBase->delete = &deleteTextWidget;

    t->uiBase->object = t;
    t->uiBase->widget = t->widget;
    t->uiBase->type = UI_TYPE_TABWIDGET;
}
void renderTextWidget(TextWidget* t){
    attron(COLOR_PAIR(t->bgColor));
    mvprintw(t->widget->topLeftY, t->widget->topLeftX, "%s", t->str);
    attroff(COLOR_PAIR(t->bgColor));
}
void updateTextWidget(TextWidget* t){
    updateWidgetTopLeft(t->widget);
}
void deleteTextWidget(TextWidget* t){
    free(t->uiBase);
    free(t->widget);
    free(t);
}