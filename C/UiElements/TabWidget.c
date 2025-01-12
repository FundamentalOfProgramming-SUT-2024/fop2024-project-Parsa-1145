#include<stdlib.h>
#include<string.h>

#include "TabWidget.h"



void createTabWidget(TabWidget* t, Widget* parent, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w, int h, int bgColor){
    t->widget = malloc(sizeof(Widget));

    createWidget(t->widget, parent, sizePolicyX, sizePolicyY, alignmentX, alignmentY, x, y, w, h, COLOR_BLACK);
    t->widget->bordered = 1;
    t->widget->layoutType = 2;


    t->bgColor = bgColor;

    t->active = NULL;
    t->Tabs = malloc(sizeof(LinkedList));
    t->tabButtons = malloc(sizeof(LinkedList));
    t->tabArea = malloc(sizeof(Widget));

    createLinkedList(t->Tabs, sizeof(TabStruct*));
    createLinkedList(t->tabButtons, sizeof(Button*));
    createWidget(t->tabArea, t->widget, RELATIVE, RELATIVE, ABSOLUTE, ABSOLUTE, 0, 0, 100, 100, C_BG_BLACK);

    t->tabArea->bordered = 1;

    t->uiBase = malloc(sizeof(UiBase));
    t->uiBase->render = &tabWidgetRender;
    t->uiBase->keyPress = &tabWidgetKeyPress;
    t->uiBase->mouseClick = &tabWidgetMouseClick;
    t->uiBase->mouseMove = &tabWidgetMouseMove;
    t->uiBase->update = &updateTabWidget;

    t->uiBase->object = t;
    t->uiBase->widget = t->widget;
    t->uiBase->type = UI_TYPE_TEXTBOX;
}
void tabWidgetAddTab(TabWidget* t, char* name, Widget* w, void(*callback)()){
    TabStruct* tmp = malloc(sizeof(TabStruct));
    w->parent = t->tabArea;
    tmp->tabWidget = t;
    tmp->tab = w;
    tmp->enterCallback = callback;
    linkedListPushBack(t->Tabs, tmp);

    Button* btn = malloc(sizeof(Button));

    createButton(btn, t->widget, name, ABSOLUTE, ABSOLUTE, ALIGN_TOP, 0, 1, strlen(name));
    btn->contextObject = tmp;
    btn->contextCallback = &tabWidgetSwitchTab;
    linkedListPushBack(t->tabButtons, btn->uiBase);
}
int tabWidgetMouseMove(TabWidget* t){
    t->tmpIterPtr = t->tabButtons->data;
    while(t->tmpIterPtr){
        t->iterPtr = t->tmpIterPtr[1];
        t->iterPtr->mouseMove(t->iterPtr->object);
        t->tmpIterPtr = t->tmpIterPtr[0];
    }
    if(t->active) WMouseMoveCb(t->active);
}
int tabWidgetMouseClick(TabWidget* t){
    t->tmpIterPtr = t->tabButtons->data;
    while(t->tmpIterPtr){
        t->iterPtr = t->tmpIterPtr[1];
        t->iterPtr->mouseClick(t->iterPtr->object);
        t->tmpIterPtr = t->tmpIterPtr[0];
    }
    if(t->active)WMouseClickCb(t->active);
}
int tabWidgetKeyPress(TabWidget* t, int ch){
    t->tmpIterPtr = t->tabButtons->data;
    while(t->tmpIterPtr){
        t->iterPtr = t->tmpIterPtr[1];
        t->iterPtr->keyPress(t->iterPtr->object, ch);
        t->tmpIterPtr = t->tmpIterPtr[0];
    }
    if(t->active)WKeyPressCb(t->active, ch);
}
int tabWidgetScroll(TabWidget* t){

}
void tabWidgetRender(TabWidget* t){
    
    t->tmpIterPtr = t->tabButtons->data;
    while(t->tmpIterPtr){
        t->iterPtr = t->tmpIterPtr[1];
        t->iterPtr->render(t->iterPtr->object);
        addch(ACS_VLINE);
        t->tmpIterPtr = t->tmpIterPtr[0];
    }
    mvaddch(t->widget->topLeftY + 1, t->widget->topLeftX, ACS_HLINE);
    FOR(i, t->widget->wCopy-1){
        addch(ACS_HLINE);
    }

    if(t->active){
        renderWidget(t->active);
    }
    //renderWidget(t->tabArea);
    //renderWidget(t->widget);

}
void tabWidgetSwitchTab(TabStruct* t){
    if(t->tabWidget->active) t->tabWidget->isVisible = 0;
    t->tabWidget->active = t->tab;
    t->tabWidget->active->isVisible = 1;
    if(t->enterCallback) t->enterCallback();
    //updateWidgetChildren(t->tab);
}
void updateTabWidget(TabWidget* t){
    updateWidgetTopLeft(t->widget);
    t->tabArea->topLeftX = t->widget->topLeftX;
    t->tabArea->topLeftY = t->widget->topLeftY + 2;
    t->tabArea->wCopy = t->widget->wCopy;
    t->tabArea->hCopy = t->widget->hCopy - 2;

    t->tmpIterPtr = t->tabButtons->data;
    int tmp = t->tabArea->topLeftX;
    while(t->tmpIterPtr){
        t->iterPtr = t->tmpIterPtr[1];
        t->tmpWidget = t->iterPtr->widget;
        t->tmpWidget->topLeftX = tmp;
        t->tmpWidget->topLeftY = t->widget->topLeftY;
        tmp += t->tmpWidget->w + 1;
        t->tmpIterPtr = t->tmpIterPtr[0];
    }
    
    if(t->active){
        updateWidgetTopLeft(t->active);
        updateWidgetChildren(t->active);
    }
}