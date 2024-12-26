#ifndef ENGINESTATE
#define ENGINESTATE

#include "../GlobalDefines.h"


    typedef struct{
        voidCB enter;
        voidCB update;
        voidCB render;
        voidCB exit;
    }EngineState;

#endif