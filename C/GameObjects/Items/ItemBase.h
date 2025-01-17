#ifndef ITEMBASE
#define ITEMBASE

#include "../Camera.h"
#include "../Texture.h"

#include "../../UiElements/Button.h"
#include "../../UiElements/TextBox.h"
#include "../../UiElements/Checkbox.h"
#include "../../UiElements/UiBase.h"
#include "../../UiElements/TabWidget.h"
#include "../../UiElements/TextWidget.h"

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
    objectCb openItemInfo;

    int *x, *y, *z;
    wchar_t* sprite;
    int* quantity;
    char* name;
}ItemBase;

void defaultItemPickup(ItemBase* g);
void defaultItemDrop(ItemBase* g);
void defaultItemRender(ItemBase* g, CharTexture* frameBuffer, ColorTexture* colorBuffer,  Camera* cam);
void defaultItemUpdate(ItemBase* g);
void noPickUp(ItemBase* g);
void noDrop(ItemBase* g);

#endif