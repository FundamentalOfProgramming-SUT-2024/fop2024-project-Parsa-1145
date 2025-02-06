#ifndef UIELEMENT
#define UIELEMENT
#include "UiType.h"
#include "../GameObjects/Renderer.h"


struct Widget;



typedef struct{
    void* object;
    struct Widget* widget;
    
    int type;
    int hovered;
    int* z;

    void (*render)(void*);
    int (*mouseClick)(void*);
    int (*mouseMove)(void*);
    int (*keyPress)(void*, int);
    void (*update)(void*);
    int (*isHovered)(void*);
    void (*delete)(void*);
}UiBase;

int defaultMouseClickCb(void*);
int defaultMouseMoveCb(void*);
int defaultKeyPressCb(void*);
void defaultRender(void*);
int defaultIsHovered(UiBase*);
void deleteUiBase(UiBase* o);

extern UiBase* hoveredElement;
extern int hoveredZ;
extern CharTexture* uiFrameBuffer;



#endif