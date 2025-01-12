#include <stdlib.h>
#include <string.h>

#include "Weapon.h"
#include "../../ProgramStates/MainGame.h"
#include "../../Globals.h"
#include "ItemTypes.h"


ItemBase* generateWeapon(cJSON* data){
    cJSON* weapon = cJSON_GetArrayItem(data, chooseWithWeight(data));

    Weapon* w = malloc(sizeof(Weapon));

    w->quantity = 1;
    w->name = copyString(cJSON_GetObjectItem(weapon, "name")->valuestring);
    w->damage = randBetween(cJSON_GetObjectItem(weapon, "minDmg")->valuedouble, cJSON_GetObjectItem(weapon, "maxDmg")->valuedouble, 0);
    w->sprite = (cJSON_GetObjectItem(weapon, "sprite")->valueint);

    createWeapon(w);

    return w->gameObject;
}
void createWeapon(Weapon* weapon){
    weapon->gameObject = malloc(sizeof(ItemBase));

    weapon->gameObject->x = &(weapon->x);
    weapon->gameObject->y = &(weapon->y);
    weapon->gameObject->z = &(weapon->z);
    weapon->gameObject->sprite = &(weapon->sprite);
    weapon->gameObject->quantity = &(weapon->quantity);
    weapon->gameObject->name = weapon->name;
    



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
    free(w->name);
    free(w->gameObject);
    free(w);
}

int isWeaponEqual(Weapon* w1, Weapon* w2){
    return (!strcmp(w1->name, w2->name));
}