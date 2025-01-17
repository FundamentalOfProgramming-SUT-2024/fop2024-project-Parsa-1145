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
    widget->scrollOn = 0;
    widget->canScrollUp = 0;

    widget->children = malloc(sizeof(LinkedList));
    createLinkedList(widget->children, sizeof(UiBase*));
    widget->layoutOffset = 0;
    widget->layoutPadding = 1;
    widget->layoutType = 0;
    widget->outOfBound = 0;

    widget->uiBase = malloc(sizeof(UiBase));
    widget->uiBase->keyPress = &WKeyPressCb;
    widget->uiBase->render = &renderWidget;
    widget->uiBase->mouseClick = &WMouseClickCb;
    widget->uiBase->mouseMove = &WMouseMoveCb;
    widget->uiBase->update = &updateWidgetChildren;
    widget->uiBase->delete = &deleteWidget;
    widget->uiBase->object = widget;
    widget->uiBase->widget = widget;
    widget->uiBase->type = UI_TYPE_WIDGET;
}
int isWidgetVisible(Widget* widget){
    if(widget->parent != NULL){
        if(isWidgetVisible(widget->parent) && widget->isVisible && (!(widget->outOfBound))) return 1;
        else return 0;
    }else{
        return widget->isVisible && (!(widget->outOfBound));
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
                widget->topLeftY = max((y + ph) - (widget->y) - (widget->hCopy), y+1);
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
                }else if(widget->alignment[1] == ALIGN_BOTTOM){
                    if(widget->hCopy + widget->y > h){
                        widget->hCopy = h - widget->y - 1;
                    }
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
    if(w->children->size){
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
            w->totalScrollArea = 0;
            w->canScrollUp = 0;
            w->scrollBarHeight = 0;
            w->scrollBarPos = 0;

            int outOfBound = 0, margin = 0, num = 0;
            int tmp = w->topLeftY - w->layoutOffset;

            w->tmpIterPtr = w->children->data;
            w->iterPtr = w->tmpIterPtr[1];
            if(w->iterPtr){
                ch = w->iterPtr->widget;
                margin = ch->y;
            }
            while(w->tmpIterPtr){
                w->iterPtr = w->tmpIterPtr[1];
                ch = w->iterPtr->widget;
                if((ch->alignment[1] == WITH_PARENT)){
                    if(outOfBound){
                        ch->outOfBound = 1;
                        w->totalScrollArea += ch->hCopy + ch->y + w->layoutPadding * (num!=0);
                    }else{
                        if(ch->isVisible){
                            ch->topLeftY = tmp + ch->y;
                            tmp += ch->hCopy + ch->y + w->layoutPadding;
                            w->totalScrollArea += ch->hCopy + ch->y + w->layoutPadding * (num!=0);
                            num++;
                        }
                        if((ch->topLeftY < w->topLeftY + margin)){
                            ch->outOfBound = 1;
                        }else if(ch->topLeftY + ch->hCopy > w->topLeftY + w->hCopy){
                            ch->outOfBound = 1;
                            w->canScrollUp = 1;
                            outOfBound = 1;
                        }else{
                            ch->outOfBound = 0;
                        }
                    }
                }
                w->tmpIterPtr = w->tmpIterPtr[0];
            }

            w->scrollBarHeight = (((float)w->hCopy) / ((float)w->totalScrollArea)) * (float)(w->hCopy-1);
            w->scrollBarPos = (((float)w->layoutOffset) / ((float)w->totalScrollArea)) * ((float)w->hCopy-1);
        }
        w->tmpIterPtr = w->children->data;
        while(w->tmpIterPtr){
            w->iterPtr = w->tmpIterPtr[1];
            w->tmpIterPtr = w->tmpIterPtr[0];
            if(w->iterPtr->type == UI_TYPE_WIDGET){
                updateWidgetChildren(w->iterPtr->object);
            }
        }
    }
}
void deleteWidget(Widget* w){
    w->tmpIterPtr = w->children->data;
    while(w->tmpIterPtr){
        w->iterPtr = w->tmpIterPtr[1];
        w->tmpIterPtr = w->tmpIterPtr[0];
        w->iterPtr->delete(w->iterPtr->object);
    }
    emptyLinkedList(w->children);
    free(w->uiBase);
    free(w->children);
    w->children = NULL;
    free(w);
}
int WMouseMoveCb(Widget* w){
    w->tmpIterPtr = w->children->data;
    while(w->tmpIterPtr){
        w->iterPtr = w->tmpIterPtr[1];
        w->tmpIterPtr = w->tmpIterPtr[0];
        w->iterPtr->mouseMove(w->iterPtr->object);
    }
}
int WMouseClickCb(Widget* w){
    int scroll = (mEvent.bstate & BUTTON5_PRESSED) || (mEvent.bstate & BUTTON4_PRESSED);
    int recieved = 0;
    w->tmpIterPtr = w->children->data;
    while(w->tmpIterPtr){
        w->iterPtr = w->tmpIterPtr[1];
        w->tmpIterPtr = w->tmpIterPtr[0];
        if(w->iterPtr->mouseClick(w->iterPtr->object)) recieved = 1;
    }
    if(scroll && w->scrollOn){
        if(!recieved){
            if(isWidgetHovered(w, mEvent.x, mEvent.y)){
                if(mEvent.bstate & BUTTON5_PRESSED){
                    if(w->canScrollUp){
                        w->layoutOffset += 1;
                        return 1;
                    }
                }else if(mEvent.bstate & BUTTON4_PRESSED){
                    if(w->layoutOffset){
                        w->layoutOffset--;
                        return 1;
                    } 
                }
                w->bordered != w->bordered;
            }
        }
    }
    return 0;
}
int WKeyPressCb(Widget* w, int key){
    w->tmpIterPtr = w->children->data;
    while(w->tmpIterPtr){
        w->iterPtr = w->tmpIterPtr[1];
        w->tmpIterPtr = w->tmpIterPtr[0];
        w->iterPtr->keyPress(w->iterPtr->object, key);
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
        if(w->scrollOn && (w->totalScrollArea > w->hCopy)){
            FOR(i, w->hCopy-2){
                mvaddch(w->topLeftY + i + 1, w->topLeftX + w->wCopy - 1, ACS_VLINE);
            }
            attron(A_REVERSE);
            FOR(i, w->scrollBarHeight){
                mvaddch(w->topLeftY + i + 1 + w->scrollBarPos, w->topLeftX + w->wCopy - 1, ' ');
            }
            attroff(A_REVERSE);
        }
        w->tmpIterPtr = w->children->data;
        while(w->tmpIterPtr){
            w->iterPtr = w->tmpIterPtr[1];
            w->iterPtr->render(w->iterPtr->object);
            w->tmpIterPtr = w->tmpIterPtr[0];
        }
    }
   
}

void emptyWidget(Widget* w){
    w->tmpIterPtr = w->children->data;
    while(w->tmpIterPtr){
        w->iterPtr = w->tmpIterPtr[1];
        w->tmpIterPtr = w->tmpIterPtr[0];
        w->iterPtr->delete(w->iterPtr->object);
    }
    emptyLinkedList(w->children);

}