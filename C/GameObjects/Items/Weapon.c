#include <stdlib.h>

#include "Weapon.h"
#include "../../ProgramStates/MainGame.h"
#include "../../Globals.h"
#include "ItemTypes.h"

void createWeapon(Weapon* weapon){
    weapon->gameObject = malloc(sizeof(ItemBase));

    weapon->gameObject->x = &(weapon->x);
    weapon->gameObject->y = &(weapon->y);
    weapon->gameObject->z = &(weapon->z);
    weapon->gameObject->sprite = &(weapon->sprite);
    weapon->gameObject->quantity = &(weapon->quantity);



    weapon->gameObject->object = weapon;
    weapon->gameObject->objectType = TYPE_WEAPON;

    weapon->gameObject->render = &defaultItemRender;
    weapon->gameObject->drop = &defaultItemDrop;
    weapon->gameObject->pickUp = &defaultItemPickup;
    weapon->gameObject->update = &defaultItemUpdate;
    weapon->gameObject->deleteObject = &deleteWeapon;
    weapon->gameObject->isEqual = &isWeaponEqual;
}
void pickUpWeapon(Weapon* w){
    removeItemFromLinkedList(floors[player.z].itemList, w->gameObject);
    linkedListPushBack(&(player.items), w->gameObject);
}
void dropWeapon(Weapon* w){

    w->x = player.x;
    w->y = player.y;
    linkedListPushBack(floors[player.z].itemList, w->gameObject);
    removeItemFromLinkedList(&(player.items), w->gameObject);
}
void renderWeapon(Weapon* w, CharTexture* frameBuffer, ColorTexture* colorBuffer, Camera* cam){

    if(isinRect(w->x, w->y, cam->x, cam->y, cam->w, cam->h)){
        frameBuffer->data[w->y - cam->y][w->x - cam->x] = w->sprite;
    }
}
void updateWeapon(Weapon* w){
    if((player.x == w->x) && (player.y == w->y)){
        pickUpWeapon(w);
    }
}
void deleteWeapon(Weapon* w){
    free(w->gameObject);
    free(w);
}

void isWeaponEqual(Weapon* w1, Weapon* w2){
    return (w1->weaponType == w2->weaponType);
}