#ifndef ENGINESTATE
#define ENGINESTATE

#include "../GlobalDefines.h"
#include "../Utilities/LinkedList.h"


typedef struct{
    voidCB enter;
    void (*update)(int);
    voidCB render;
    voidCB exit;

    LinkedList* uiList;
}EngineState;

#endif