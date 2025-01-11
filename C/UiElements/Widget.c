#include <stdlib.h>
#include <math.h>

#include "Widget.h"
#include "../Globals.h"
#include"../Utilities/LinkedList.h"
#include "../GlobalDefines.h"


void createWidget(Widget* widget,Widget* parent, int sizeTypeX, int sizeTypeY, int alignmentX, int alignmentY , int x, int y, int w, int h, int bgColor){
    widget->bgColor = bgColor;

    widget->w = w;
    widget->h = h;
    widget->x = x;
    widget->y = y;

    widget->sizeType[0] = sizeTypeX;
    widget->sizeType[1] = sizeTypeY;

    widget->alignment[0] = alignmentX;
    widget->alignment[1] = alignmentY;

    widget->parent = parent;
    updateWidgetSize(widget);

    widget->colorPair = bgColor;
    widget->bordered = 0;

    widget->isVisible = 1;
    widget->layoutType = 0;

    widget->children = malloc(sizeof(LinkedList));
    createLinkedList(widget->children, sizeof(UiBase*));
    widget->layoutOffset = 0;
    widget->layoutPadding = 1;
    widget->layoutType = 0;

    widget->uiBase = malloc(sizeof(UiBase));
    widget->uiBase->keyPress = &WKeyPressCb;
    widget->uiBase->render = &renderWidget;
    widget->uiBase->mouseClick = &WMouseClickCb;
    widget->uiBase->mouseMove = &WMouseMoveCb;
    widget->uiBase->update = &updateWidgetChildren;
    widget->uiBase->object = widget;
    widget->uiBase->widget = widget;
    widget->uiBase->type = UI_TYPE_WIDGET;
}
int isWidgetVisible(Widget* widget){
    if(widget->parent != NULL){
        if(isWidgetVisible(widget->parent) && widget->isVisible) return 1;
        else return 0;
    }else{
        return widget->isVisible;
    }
}
int updateWidgetTopLeft(Widget* widget){
    if(widgetParentResized(widget)){
        int pw, ph, x, y;

        if(widget->parent != NULL){
            x = widget->parent->topLeftX;
            y = widget->parent->topLeftY;
        }else{
            x = 0;
            y = 0;
        }

        updateWidgetSize(widget);
        

        widget->scrHCopy = scrH;
        widget->scrWCopy = scrW;


        if(widget->parent != NULL){
            pw = widget->parent->wCopy;
            ph = widget->parent->hCopy;
        }else{
            pw = scrW;
            ph = scrH;
        }

        switch(widget->alignment[0]){
            case ABSOLUTE:
                widget->topLeftX = x + widget->x;
                break;
            case ALIGN_LEFT:
                widget->topLeftX = x + widget->x;
                break;
            case ALIGN_CENTER:
                widget->topLeftX = x + ((pw - widget->wCopy) / 2);
                break;
            case ALIGN_RIGHT:
                widget->topLeftX = x + pw - widget->wCopy - widget->x;
                break;
        }
        switch(widget->alignment[1]){
            case ABSOLUTE:
                widget->topLeftY = y + widget->y;
                break;
            case ALIGN_TOP:
                widget->topLeftY = y + widget->y;
                break;
            case ALIGN_CENTER:
                widget->topLeftY = y + ((ph - widget->hCopy) / 2);
                break;
            case ALIGN_BOTTOM:
                widget->topLeftY = (y + ph) - (widget->y) - (widget->hCopy);
                break;
            case WITH_PARENT:
                break;
        }
        return 1;
    }else{
        return 0;
    }
}

int updateWidgetSize(Widget* widget){
    if(widgetParentResized(widget)){
        int w, h;
        widget->scrHCopy = scrH;
        widget->scrWCopy = scrW;

        if(widget->parent != NULL){
            w = widget->parent->wCopy;
            h = widget->parent->hCopy;
        }else{
            w = scrW;
            h = scrH;
        }

        switch(widget->sizeType[0]){
            case ABSOLUTE:
                widget->wCopy = min(w, widget->w);
                if(widget->x + widget->wCopy > w) widget->wCopy = w - widget->x;
                break;
            case RELATIVE:
                widget->wCopy = ((w * widget->w ) / 100);
                if(widget->alignment[0] == ALIGN_CENTER){
                    widget->wCopy -= 2 * widget->x;
                    widget->wCopy += (widget->wCopy^w)&1;
                }
                break;
        }switch(widget->sizeType[1]){
            case ABSOLUTE:
                widget->hCopy = min(h, widget->h);
                break;
            case RELATIVE:
                widget->hCopy = round((h * widget->h) / 100);
                if(widget->alignment[1] == ALIGN_CENTER){
                    widget->hCopy -= 2 * widget->y;
                    widget->hCopy += (widget->hCopy^h)&1;
                }
                break;
        }
        return 1;

    }else{
        return 0;
    }
}
int widgetParentResized(Widget* widget){          /////////// right now it only accounts for window resize which is ok, but it would be neater to account for parent resize
    return 1;
}
int isWidgetHovered(Widget* widget, int x, int y){
    return (((x >= widget->topLeftX) && (x < widget->topLeftX + widget->wCopy)) && ((y >= widget->topLeftY) && (y < widget->topLeftY + widget->hCopy)));
}

void updateWidgetChildren(Widget* w){
    if(!(w->parent)){
        updateWidgetTopLeft(w);
    }
    w->tmpIterPtr = w->children->data;
    Widget* ch;
    while(w->tmpIterPtr){
        w->iterPtr = w->tmpIterPtr[1];
        if(w->iterPtr->type == UI_TYPE_WIDGET){
            updateWidgetTopLeft(w->iterPtr->object);
        }else{
            w->iterPtr->update(w->iterPtr->object);
        }
        w->tmpIterPtr = w->tmpIterPtr[0];
    }
    w->tmpIterPtr = w->children->data;
    if(w->layoutType == VERTICAL_LAYOUT){
        w->tmpIterPtr = w->children->data;
        int tmp = w->topLeftY;
        while(w->tmpIterPtr){
            w->iterPtr = w->tmpIterPtr[1];
            ch = w->iterPtr->widget;
            if((ch->alignment[1] == WITH_PARENT) && (ch->isVisible)){
                ch->topLeftY = tmp + ch->y;
                tmp += ch->hCopy + ch->y + w->layoutPadding;
            }
            w->tmpIterPtr = w->tmpIterPtr[0];
        }
    }
    w->tmpIterPtr = w->children->data;
    while(w->tmpIterPtr){
        w->iterPtr = w->tmpIterPtr[1];
        if(w->iterPtr->type == UI_TYPE_WIDGET){
            updateWidgetChildren(w->iterPtr->object);
        }
        w->tmpIterPtr = w->tmpIterPtr[0];
    }
}
void deleteWidget(Widget* widget){
    deleteLinkedList(widget->children);
    free(widget->uiBase);
    free(widget->children);
    widget->children = NULL;
    free(widget);
}
int WMouseMoveCb(Widget* w){
    w->tmpIterPtr = w->children->data;
    while(w->tmpIterPtr){
        w->iterPtr = w->tmpIterPtr[1];
        w->iterPtr->mouseMove(w->iterPtr->object);
        w->tmpIterPtr = w->tmpIterPtr[0];
    }
}
int WMouseClickCb(Widget* w){
    w->tmpIterPtr = w->children->data;
    while(w->tmpIterPtr){
        w->iterPtr = w->tmpIterPtr[1];
        w->iterPtr->mouseClick(w->iterPtr->object);
        w->tmpIterPtr = w->tmpIterPtr[0];
    }
}
int WKeyPressCb(Widget* w, int key){
    w->tmpIterPtr = w->children->data;
    while(w->tmpIterPtr){
        w->iterPtr = w->tmpIterPtr[1];
        w->iterPtr->keyPress(w->iterPtr->object, key);
        w->tmpIterPtr = w->tmpIterPtr[0];
    }
}
void renderWidget(Widget* w){
    if(isWidgetVisible(w)){
        if(w->bordered){
            move(w->topLeftY, w->topLeftX);
            add_wch(WACS_D_ULCORNER);
            FOR(i, w->wCopy-2){
                add_wch(WACS_D_HLINE);
            }
            add_wch(WACS_D_URCORNER);
            move(w->topLeftY + w->hCopy-1, w->topLeftX);
            add_wch(WACS_D_LLCORNER);
            FOR(i, w->wCopy-2){
                add_wch(WACS_D_HLINE);
            }
            add_wch(WACS_D_LRCORNER);
            for(int i = w->topLeftY + 1; i < w->topLeftY + w->hCopy-1; i++){
                move(i, w->topLeftX);
                addch(ACS_VLINE);
                move(i, w->topLeftX + w->wCopy-1);
                addch(ACS_VLINE);
            }
        }
        
        if(w->colorPair){
            attron(COLOR_PAIR(w->colorPair));
            for(int i = w->topLeftY + 1; i < w->topLeftY + w->hCopy-1; i++){
                move(i, w->topLeftX+1);
                for(int j = w->topLeftX + 1; j < w->topLeftX + w->wCopy - 1; j++){
                    addch(' ');
                }
            }
            attroff(COLOR_PAIR(w->colorPair));
        }
        w->tmpIterPtr = w->children->data;
        while(w->tmpIterPtr){
            w->iterPtr = w->tmpIterPtr[1];
            w->iterPtr->render(w->iterPtr->object);
            w->tmpIterPtr = w->tmpIterPtr[0];
        }
    }
   
}