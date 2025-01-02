#ifndef GAMEOBJECT
#define GAMEOBJECT

#include "Camera.h"
#include "Texture.h"

struct GameObject;

typedef void (*objectCb)(struct GameObject*);

typedef struct GameObject{
    void* object;
    objectCb update;
    void (*render)(void*, CharTexture*, ColorTexture*, Camera*);
    objectCb pickUp;
    objectCb drop;
    objectCb deleteObject;

    int *x, *y, *z;



}GameObject;

#endif