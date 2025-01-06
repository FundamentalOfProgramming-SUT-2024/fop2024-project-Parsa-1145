#include "ItemBase.h"
#include "../../ProgramStates/MainGame.h"
#include "../../Globals.h"

void defaultItemPickup(ItemBase* g){
    removeItemFromLinkedList(floors[player.z].itemList, g);

    ItemBase* tmp;
    FOR(i, player.items.size){
        tmp = linkedListGetElement(&player.items, i);

        if(tmp->objectType == g->objectType){
            if(g->isEqual(tmp->object, g->object)){
                *(tmp->quantity) += *(g->quantity);
                g->deleteObject(g->object);
                return;
            }
        }
    }

    linkedListPushBack(&(player.items), g);
}
void defaultItemDrop(ItemBase* g){
    *(g->x) = player.x;
    *(g->y) = player.y;
    linkedListPushBack(floors[player.z].itemList, g);
    removeItemFromLinkedList(&(player.items), g);
}
void defaultItemRender(ItemBase* g, CharTexture* frameBuffer, ColorTexture* colorBuffer, Camera* cam){

    if(isinRect(*(g->x), *(g->y), cam->x, cam->y, cam->w, cam->h)){
        frameBuffer->data[*(g->y) - cam->y][*(g->x) - cam->x] = *(g->sprite);
    }
}
void defaultItemUpdate(ItemBase* g){

    if((player.x == *(g->x)) && (player.y == *(g->y))){
        defaultItemPickup(g);
    }
}
void noPickUp(ItemBase* g){}
void noDrop(ItemBase* g){}