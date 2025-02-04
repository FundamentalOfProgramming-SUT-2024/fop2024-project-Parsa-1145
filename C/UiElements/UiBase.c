#include "UiBase.h"
#include "Widget.h"
#include "../Globals.h" 

int defaultMouseClickCb(void*){return 0;}
int defaultMouseMoveCb(void*){return 0;}
int defaultKeyPressCb(void*){return 0;}
void defaultRender(void*){}
int defaultIsHovered(UiBase* o){
    if(isWidgetHovered(o->widget, mEvent.x, mEvent.y)){
        if(o->widget->z >= hoveredZ){
            hoveredElement = o;
            hoveredZ = o->widget->z;
        }
    }else{
        if(hoveredElement == o){
            hoveredElement = NULL;
            hoveredZ = 0;
        }
    }
}

UiBase* hoveredElement = NULL;
int hoveredZ = -1;
