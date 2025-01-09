#ifndef DOOR
#define DOOR
#include "ItemBase.h"
#include "../Camera.h"
#include "../Texture.h"

typedef struct{
    ItemBase* itemBase;

    int x, y, z;
    wchar_t sprite;
    int color;

    int hidden;
    int locked;
}Door;

void createDoor(Door* d);
void renderDoor(ItemBase* o, CharTexture* frameBuffer, ColorTexture* colorBuffer, Camera* cam);
void updateDoor(ItemBase* o);
void deleteDoor(ItemBase* o);


#endif