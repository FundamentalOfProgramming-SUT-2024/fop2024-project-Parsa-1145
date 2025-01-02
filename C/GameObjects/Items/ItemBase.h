#ifndef ITEMBASE
#define ITEMBASE

#include "../Camera.h"
#include "../Texture.h"

struct ItemBase;

typedef void (*objectCb)(struct ItemBase*);

typedef struct ItemBase{
    void* object;
    int objectType;
    objectCb update;
    void (*render)(void*, CharTexture*, ColorTexture*, Camera*);
    int (*isEqual)(void*, void*);
    objectCb pickUp;
    objectCb drop;
    objectCb deleteObject;

    int *x, *y, *z;
    char* sprite;
    int* quantity;
}ItemBase;

void defaultItemPickup(ItemBase* g);
void defaultItemDrop(ItemBase* g);
void defaultItemRender(ItemBase* g, CharTexture* frameBuffer, ColorTexture* colorBuffer,  Camera* cam);
void defaultItemUpdate(ItemBase* g);

#endif