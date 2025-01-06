#include<stdio.h>
#include "Door.h"
#include "../../ProgramStates/MainGame.h"
#include "ItemTypes.h"
#include "../../Globals.h"
void createDoor(Door* d){
    d->itemBase = malloc(sizeof(ItemBase));

    d->itemBase->x = &(d->x);
    d->itemBase->y = &(d->y);
    d->itemBase->z = &(d->z);
    d->itemBase->sprite = &(d->sprite);



    d->itemBase->object = d;
    d->itemBase->objectType = TYPE_DOOR;

    d->itemBase->render = &renderDoor;
    d->itemBase->drop = &noDrop;
    d->itemBase->pickUp = &noPickUp;
    d->itemBase->update = &updateDoor;
    d->itemBase->deleteObject = &deleteDoor;
    d->itemBase->isEqual = NULL;
}
void renderDoor(ItemBase* o, CharTexture* frameBuffer, ColorTexture* colorBuffer, Camera* cam){
    Door* d = o->object;
    if(!(d->hidden)){
        if(isinRect(d->x, d->y, cam->x, cam->y, cam->w, cam->h)){
            frameBuffer->data[d->y - cam->y][d->x - cam->x] = d->sprite;
        }
    }
}
void updateDoor(ItemBase* o){
    Door* d = o->object;

    if((player.x == d->x) && (player.y == d->y)){
        d->hidden = 0;
    }
}
void deleteDoor(ItemBase* o){
    Door* d = o->object;

    free(d->itemBase);
    free(d);
}
