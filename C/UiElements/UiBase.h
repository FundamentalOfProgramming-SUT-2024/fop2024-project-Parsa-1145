#ifndef UIELEMENT
#define UIELEMENT
#include "UiType.h"

typedef struct{
    void* object;
    void* widget;
    
    int type;

    void (*render)(void*);
    int (*mouseClick)(void*);
    int (*mouseMove)(void*);
    int (*keyPress)(void*, int);
    void (*update)(void*);
    void (*delete)(void*);
}UiBase;

int defaultMouseClickCb(void*);
int defaultMouseMoveCb(void*);
int defaultKeyPressCb(void*);
void defaultRender(void*);


#endif