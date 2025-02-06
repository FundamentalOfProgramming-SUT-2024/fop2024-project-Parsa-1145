#include<stdlib.h>
#include<string.h>

#include "TabWidget.h"



void createTabWidget(TabWidget* t, Widget* parent, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w, int h, int bgColor){
    t->widget = calloc(1, sizeof(Widget));

    createWidget(t->widget, parent, sizePolicyX, sizePolicyY, alignmentX, alignmentY, x, y, w, h, NULL);
    t->widget->bordered = 1;
    t->widget->layoutType = 2;


    if(bgColor){
        t->bgColor = bgColor;
    }else{
        if(parent){
            t->bgColor = getTopColor(parent);
        }
    }

    t->active = NULL;
    t->Tabs = malloc(sizeof(LinkedList));
    t->tabButtons = malloc(sizeof(LinkedList));
    t->tabArea = malloc(sizeof(Widget));

    createLinkedList(t->Tabs, sizeof(TabStruct*));
    createLinkedList(t->tabButtons, sizeof(Button*));
    createWidget(t->tabArea, t->widget, RELATIVE, RELATIVE, ABSOLUTE, ABSOLUTE, 0, 0, 100, 100, NULL);

    t->tabArea->bordered = 1;

    t->uiBase = malloc(sizeof(UiBase));
    t->uiBase->render = &tabWidgetRender;
    t->uiBase->keyPress = &tabWidgetKeyPress;
    t->uiBase->mouseClick = &tabWidgetMouseClick;
    t->uiBase->mouseMove = &tabWidgetMouseMove;
    t->uiBase->update = &updateTabWidget;
    t->uiBase->isHovered = &isTabWidgetHovered;
    t->uiBase->z = &(t->widget->z);

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
    if(t->active){
        t->active->uiBase->mouseMove(t->active);
    }
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
        color_set(t->bgColor, NULL);
        if(t->tmpIterPtr[0]){
            addWchToFrameBuffer(uiFrameBuffer, WACS_VLINE->chars[0], t->widget->z, t->bgColor, 0);
        }
        color_set(0, NULL);
        t->tmpIterPtr = t->tmpIterPtr[0];
    }

    color_set(t->bgColor, NULL);
    moveInFrameBuffer(uiFrameBuffer, t->widget->topLeftY + 1, t->widget->topLeftX);
    FOR(i, t->widget->wCopy){
        addWchToFrameBuffer(uiFrameBuffer, WACS_HLINE->chars[0], t->widget->z, t->bgColor, 0);
    }
    color_set(0, NULL);


    if(t->active){
        renderWidget(t->active);
    }
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
void isTabWidgetHovered(UiBase* o){
    TabWidget* t = o->object;
    t->tmpIterPtr = t->tabButtons->data;
    while(t->tmpIterPtr){
        t->iterPtr = t->tmpIterPtr[1];
        t->iterPtr->isHovered(t->iterPtr);
        t->tmpIterPtr = t->tmpIterPtr[0];
    }
    if(t->active) updateChildrenHovered(t->active->uiBase);
}