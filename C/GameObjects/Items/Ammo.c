#include <stdlib.h>
#include <string.h>

#include "Ammo.h"
#include "../../ProgramStates/MainGame.h"
#include "../../Globals.h"
#include "ItemTypes.h"

ItemBase* generateAmmo(cJSON* data){
    cJSON* ammo = cJSON_GetArrayItem(data, chooseWithWeight(data));

    Ammo* a = malloc(sizeof(Ammo));

    a->name = copyString(cJSON_GetObjectItem(ammo, "name")->valuestring);
    a->damage = cJSON_GetObjectItem(ammo, "dmg")->valuedouble;
    a->quantity = randBetween(cJSON_GetObjectItem(ammo, "minQuantity")->valuedouble, cJSON_GetObjectItem(ammo, "maxQuantity")->valuedouble, 0);
    a->sprite = (cJSON_GetObjectItem(ammo, "sprite")->valuestring)[0];

    createAmmo(a);

    return a->gameObject;
}

void createAmmo(Ammo* a){
    a->gameObject = malloc(sizeof(ItemBase));

    a->gameObject->x = &(a->x);
    a->gameObject->y = &(a->y);
    a->gameObject->z = &(a->z);
    a->gameObject->sprite = &(a->sprite);
    a->gameObject->quantity = &(a->quantity);

    a->gameObject->object = a;
    a->gameObject->objectType = TYPE_AMMO;

    a->gameObject->render = &defaultItemRender;
    a->gameObject->drop = &defaultItemDrop;
    a->gameObject->pickUp = &defaultItemPickup;
    a->gameObject->update = &defaultItemUpdate;
    a->gameObject->deleteObject = &deleteAmmo;
    a->gameObject->isEqual = &isAmmoEqual;
}

int isAmmoEqual(Ammo* a1, Ammo* a2){
    return(!strcmp(a1->name, a2->name));
}
void pickUpAmmo(Ammo* a){
    removeItemFromLinkedList(floors[player.z].itemList, a->gameObject);

    ItemBase* tmp;
    FOR(i, player.items.size){
        tmp = linkedListGetElement(&player.items, i);

        if(tmp->objectType == TYPE_AMMO){
            Ammo* w2 = tmp->object;
            if(isAmmoEqual(a, w2)){
                w2->quantity += a->quantity;
                deleteAmmo(a);
                return;
            }
        }
    }

    linkedListPushBack(&(player.items), a->gameObject);
}
void dropAmmo(Ammo* a){
    a->x = player.x;
    a->y = player.y;
    linkedListPushBack(floors[player.z].itemList, a->gameObject);
    removeItemFromLinkedList(&(player.items), a->gameObject);
}
void renderAmmo(Ammo* a, CharTexture* frameBuffer, ColorTexture* colorBuffer, Camera* cam){
    if(isinRect(a->x, a->y, cam->x, cam->y, cam->w, cam->h)){
        frameBuffer->data[a->y - cam->y][a->x - cam->x] = a->sprite;
    }
}
void updateAmmo(Ammo* a){
    if((player.x == a->x) && (player.y == a->y)){
        pickUpAmmo(a);
    }
}
void deleteAmmo(Ammo* a){
    free(a->name);
    free(a->gameObject);
    free(a);
}