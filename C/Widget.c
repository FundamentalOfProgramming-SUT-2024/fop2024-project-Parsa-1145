#include "Widget.h"
#include "Globals.h"
#include "GlobalDefines.h"


void createWidget(Widget* widget,Widget* parent, int sizeTypeX, int sizeTypeY, int alignmentX, int alignmentY , int x, int y, int w, int h, int bColor, int bgColor){
    widget->bColor = bColor;
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
    
    int tmpx, tmpy;
    getWidgetTopLeft(widget, &tmpx, &tmpy);

    widget->colorPair = colorPairNum;
    init_pair(widget->colorPair, bColor, bgColor);
    colorPairNum++;
}

int getWidgetTopLeft(Widget* widget, int*x, int* y){
    if(widgetParentResized(widget)){
        int pw, ph;

        if(widget->parent != NULL){
            getWidgetTopLeft(widget->parent, x, y);
        }else{
            *x = 0;
            *y = 0;
        }

        getWidgetSize(widget, &(widget->wCopy), &(widget->hCopy));
        

        widget->scrHCopy = scrH;
        widget->scrWCopy = scrW;


        if(widget->parent != NULL){
            getWidgetSize(widget->parent, &pw, &ph);
        }else{
            pw = scrW;
            ph = scrH;
        }

        switch(widget->alignment[0]){
            case ABSOLUTE:
                *x = *x + widget->x;
                break;
            case ALIGN_LEFT:
                *x = *x + widget->x;
                break;
            case ALIGN_CENTER:
                *x = *x + ((pw - widget->wCopy) / 2);
                break;
            case ALIGN_RIGHT:
                *x = *x + pw - widget->wCopy + widget->x;
                break;
        }

        switch(widget->alignment[1]){
            case ABSOLUTE:
                *y = *y + widget->y;
                break;
            case ALIGN_TOP:
                *y = *y + widget->y;
                break;
            case ALIGN_CENTER:
                *y = *y + ((ph - widget->hCopy) / 2);
                break;
            case ALIGN_BOTTOM:
                *y = (*y + ph) - (widget->y) - (widget->hCopy);
                break;
        }
        widget->topLeftX = *x;
        widget->topLeftY = *y;
        return 1;
    }else{
        *x = widget->topLeftX;
        *y = widget->topLeftY;
        return 0;
    }
}

int getWidgetSize(Widget* widget, int* w, int* h){
    if(widgetParentResized(widget)){

        widget->scrHCopy = scrH;
        widget->scrWCopy = scrW;

        if(widget->parent != NULL){
            getWidgetSize(widget->parent, w, h);
        }else{
            *w = scrW;
            *h = scrH;
        }

        switch(widget->sizeType[0]){
            case ABSOLUTE:
                *w = min(*w, widget->w);
                break;
            case RELATIVE:
                *w = (*w * widget->w ) / 100;
                break;
        }switch(widget->sizeType[1]){
            case ABSOLUTE:
                *h = min(*h, widget->h);
                break;
            case RELATIVE:
                *h = ((*h * widget->h) / 100) - 1;
                break;
        }

        widget->hCopy = *h;
        widget->wCopy = *w;
        return 1;

    }else{
        *w = widget->wCopy;
        *h = widget->hCopy;
        return 0;
    }
}
int widgetParentResized(Widget* widget){          /////////// right now it only accounts for window resize which is ok, but it would be neater to account for parent resize
    return (widget->scrHCopy != scrH) || (widget->scrWCopy != scrW);
}

void renderWidget(Widget* widget){
    //if(widgetParentResized(widget)){
        int tmpx, tmpy;
        getWidgetTopLeft(widget, &tmpx, &tmpy);
        attron(COLOR_PAIR(widget->colorPair));
        move(widget->topLeftY+1, widget->topLeftX+1);
        for(int i = widget->topLeftY + 1; i < widget->topLeftY + widget->hCopy; i++){
            for(int j = widget->topLeftX + 1; j < widget->topLeftX + widget->wCopy; j++){
                addch(' ');
            }
            move(i+1, widget->topLeftX + 1);
        }
        attroff(COLOR_PAIR(widget->colorPair));
    //}
}

int updateWidgetSize(Widget* widget){
    int w, h;
    return getWidgetSize(widget, &w, &h);
}
int updateWidgetTopLeft(Widget* widget){
    int x, y;
    return getWidgetTopLeft(widget, &x, &y);
}

int isWidgetHovered(Widget* widget, int x, int y){
    updateWidgetTopLeft(widget);

    return (((x >= widget->topLeftX) && (x < widget->topLeftX + widget->wCopy)) && ((y >= widget->topLeftY) && (y < widget->topLeftY + widget->hCopy)));
}