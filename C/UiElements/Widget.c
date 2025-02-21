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

    widget->overflow[0] = 0;
    widget->overflow[1] = 0;

    widget->parent = parent;
    updateWidgetSize(widget);


    widget->bgColor = bgColor;

    widget->isVisible = 1;
    widget->layoutType = 0;
    widget->scrollOn = 0;
    widget->canScrollUp = 0;
    widget->z = 0;
    widget->bordered = 0;

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
    widget->uiBase->isHovered = &widgetHoveredUpdate;
    widget->uiBase->z = &(widget->z);

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
                if(widget->overflow[0]){
                    widget->wCopy = widget->w;

                }else{
                    widget->wCopy = min(w, widget->w);
                    if(widget->x + widget->wCopy > w) widget->wCopy = w - widget->x;
                }
                break;
            case RELATIVE:
                widget->wCopy = ((w * widget->w ) / 100);
                if(widget->alignment[0] == ALIGN_CENTER){
                    widget->wCopy -= 2 * widget->x;
                    widget->wCopy += (widget->wCopy^w)&1;
                }else if(widget->alignment[0] != ABSOLUTE){
                    if(widget->wCopy + widget->x > w){
                        widget->wCopy = w - widget->x;
                    }
                }
                break;
        }switch(widget->sizeType[1]){
            case ABSOLUTE:
                if(widget->overflow[1]){
                    widget->hCopy = widget->h;
                }else{
                    widget->hCopy = min(h, widget->h);
                }
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
                }else if(widget->alignment[1] == ALIGN_TOP){
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
int widgetParentResized(Widget* widget){
    return 1;
}
int isWidgetHovered(Widget* widget, int x, int y){
    return (isWidgetVisible(widget) && ((x >= widget->topLeftX) && (x < widget->topLeftX + widget->wCopy)) && ((y >= widget->topLeftY) && (y < widget->topLeftY + widget->hCopy)));
}
int widgetHoveredUpdate(UiBase* o){
    Widget* widget = o->object;
    if(isWidgetHovered(widget, mEvent.x, mEvent.y)){
        if(widget->z >= hoveredZ){
            hoveredElement = widget->uiBase;
            hoveredZ = widget->z;
        }
        return 1;
    }else{
        return 0;
    }
}

void updateWidgetChildren(Widget* w){
    if(isWidgetVisible(w)){
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
                    margin = ch->y - 1;
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
                            if((ch->topLeftY + ch->hCopy <= w->topLeftY + w->bordered + margin)){
                                ch->outOfBound = 1;
                            }else if(ch->topLeftY >= w->topLeftY + w->hCopy - w->bordered){
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
                //w->layoutOffset = min(w->layoutOffset, w->totalScrollArea - w->hCopy);

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
        }else{
            w->canScrollUp = 0;
            w->layoutOffset = 0;
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
    deleteUiBase(w->uiBase);
    free(w->children);
    w->children = NULL;
    free(w);
}
int WMouseMoveCb(Widget* w){
    if(isWidgetVisible(w)){

        w->tmpIterPtr = w->children->data;
        int flag = 0;
        while(w->tmpIterPtr){
            w->iterPtr = w->tmpIterPtr[1];
            w->tmpIterPtr = w->tmpIterPtr[0];
            flag |= w->iterPtr->mouseMove(w->iterPtr->object);
        }
        return flag || (w->bgColor && isWidgetHovered(w, mEvent.x, mEvent.y));
    }else{
        return 0;
    }
}
int WMouseClickCb(Widget* w){
    if(isWidgetVisible(w)){
        int scroll = (mEvent.bstate & BUTTON5_PRESSED) || (mEvent.bstate & BUTTON4_PRESSED);
        int flag = 0;
        w->tmpIterPtr = w->children->data;

        while(w->tmpIterPtr){
            w->iterPtr = w->tmpIterPtr[1];
            w->tmpIterPtr = w->tmpIterPtr[0];
            flag |= w->iterPtr->mouseClick(w->iterPtr->object);
        }
        if(scroll && w->scrollOn){
            if(!flag){
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
                }
            }
        }
        if(flag){
            return 1;
        }
    }else{
        return 0;
    }
}
int WKeyPressCb(Widget* w, int key){
    if(isWidgetVisible(w)){
        w->tmpIterPtr = w->children->data;
        int flag = 0;
        while(w->tmpIterPtr){
            w->iterPtr = w->tmpIterPtr[1];
            w->tmpIterPtr = w->tmpIterPtr[0];
            flag |= w->iterPtr->keyPress(w->iterPtr->object, key);
        }
        return flag || (w->bgColor && isWidgetHovered(w, mEvent.x, mEvent.y));
    }else{
        return 0;
    }
}
void renderWidget(Widget* w){
    if(isWidgetVisible(w)){
        if(w->bgColor){
            attr_set(0, w->bgColor, NULL);
            for(int i = w->topLeftY; i < w->topLeftY + w->hCopy ; i++){
                moveInFrameBuffer(uiFrameBuffer ,i, w->topLeftX);
                for(int j = w->topLeftX; j < w->topLeftX + w->wCopy; j++){
                    addWchToFrameBuffer(uiFrameBuffer , ' ', w->z, w->bgColor, NULL);
                }
            }
            attr_set(0, 0, NULL);
        }
        if(w->bordered){
            moveInFrameBuffer(uiFrameBuffer, w->topLeftY, w->topLeftX);
            addWchToFrameBuffer(uiFrameBuffer, WACS_D_ULCORNER->chars[0], w->z, 0, 0);
            FOR(i, w->wCopy-2){
                addWchToFrameBuffer(uiFrameBuffer , WACS_D_HLINE->chars[0], w->z, 0, 0);
            }
            addWchToFrameBuffer(uiFrameBuffer , WACS_D_URCORNER->chars[0], w->z, 0, 0);
            moveInFrameBuffer(uiFrameBuffer, w->topLeftY + w->hCopy-1, w->topLeftX);
            addWchToFrameBuffer(uiFrameBuffer , WACS_D_LLCORNER->chars[0], w->z, 0, 0);
            FOR(i, w->wCopy-2){
                addWchToFrameBuffer(uiFrameBuffer , WACS_D_HLINE->chars[0], w->z, 0, 0);
            }
            addWchToFrameBuffer(uiFrameBuffer , WACS_D_LRCORNER->chars[0], w->z, 0, 0);
            for(int i = w->topLeftY + 1; i < w->topLeftY + w->hCopy-1; i++){
                moveInFrameBuffer(uiFrameBuffer, i, w->topLeftX);
                addWchToFrameBuffer(uiFrameBuffer , WACS_VLINE->chars[0], w->z, 0, NULL);
                moveInFrameBuffer(uiFrameBuffer, i, w->topLeftX + w->wCopy-1);
                addWchToFrameBuffer(uiFrameBuffer , WACS_VLINE->chars[0], w->z, 0, NULL);
            }
        }
        
        
        if(w->scrollOn && (w->totalScrollArea > w->hCopy)){
            FOR(i, w->hCopy-2){
                mvAddWchToFrameBuffer(uiFrameBuffer ,w->topLeftY + i + 1, w->topLeftX + w->wCopy - 1, WACS_VLINE->chars[0], w->z + 1, 0, NULL);
            }
            attron(A_REVERSE);
            FOR(i, w->scrollBarHeight){
                mvAddWchToFrameBuffer(uiFrameBuffer ,w->topLeftY + i + 1 + w->scrollBarPos, w->topLeftX + w->wCopy - 1, ' ', w->z + 1, 0, NULL);
            }
            attroff(A_REVERSE);
        }
        w->tmpIterPtr = w->children->data;
        while(w->tmpIterPtr){
            w->iterPtr = w->tmpIterPtr[1];
            w->iterPtr->render(w->iterPtr->object);
            w->tmpIterPtr = w->tmpIterPtr[0];
        }

        // mvFramBufferPrintW(uiFrameBuffer ,w->topLeftY, w->topLeftX, 0, "%d", w->topLeftY + w->hCopy);
        // mvFramBufferPrintW(uiFrameBuffer ,w->topLeftY + w->hCopy, w->topLeftX + w->wCopy - 5, 0, "%d", w->topLeftY + w->bordered);

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
    w->layoutOffset = 0;
    w->canScrollUp = 0;
    w->totalScrollArea = 0;

}

void updateChildrenHovered(UiBase* o){
    Widget* w = o->object;
    widgetHoveredUpdate(o);
    if(w->children->size){
        void** tmp = w->children->data;
        UiBase* ptr;

        while(tmp){
            ptr = tmp[1];
            tmp = tmp[0];

            ptr->isHovered(ptr);
            if(ptr->type == UI_TYPE_WIDGET){
                updateChildrenHovered(ptr);
            }
        }
    }
}

int getTopColor(Widget* w){
    if(w->bgColor){
        return w->bgColor;
    }else{
        if(w->parent){
            return getTopColor(w->parent);
        }else{
            return NULL;
        }
    }
}