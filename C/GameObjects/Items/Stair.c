#include<stdio.h>
#include "Stair.h"
#include "../../ProgramStates/MainGame.h"
#include "ItemTypes.h"
#include "../../Globals.h"
void createStair(Stair* s){
    s->itemBase = malloc(sizeof(ItemBase));

    s->itemBase->x = &(s->x);
    s->itemBase->y = &(s->y);
    s->itemBase->z = &(s->z);
    s->itemBase->sprite = &(s->sprite);



    s->itemBase->object = s;
    s->itemBase->objectType = TYPE_STAIR;

    s->itemBase->render = &defaultItemRender;
    s->itemBase->drop = &noDrop;
    s->itemBase->pickUp = &noPickUp;
    s->itemBase->update = &updateStair;
    s->itemBase->deleteObject = &deleteStair;
    s->itemBase->isEqual = NULL;
}
void updateStair(ItemBase* o){
    Stair* s = o->object;

    if((player.x == s->x) && (player.y == s->y)){
        player.z = s->dest->z;
        player.x = s->dest->x;
        player.y = s->dest->y;

    }
}
void deleteStair(ItemBase* o){
    Stair* s = o->object;

    free(s->itemBase);
    free(s);
}
