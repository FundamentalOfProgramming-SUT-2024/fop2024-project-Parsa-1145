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
}UiBase;

void defaultMouseClickCb(void*);
void defaultMouseMoveCb(void*);
void defaultKeyPressCb(void*);
void defaultRender(void*);


#endif