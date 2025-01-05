#include <stdlib.h>
#include <string.h>

#include "Key.h"
#include "ItemBase.h"
#include "../../ProgramStates/MainGame.h"
#include "../../Globals.h"
#include "ItemTypes.h"

ItemBase* generateKey(cJSON* data){
    cJSON* key = cJSON_GetArrayItem(data, chooseWithWeight(data));

    Key* k = malloc(sizeof(Key));

    k->name = copyString(cJSON_GetObjectItem(key, "name")->valuestring);
    k->broken = cJSON_GetObjectItem(key, "broken")->valueint;
    k->openProb = cJSON_GetObjectItem(key, "openProb")->valuedouble;
    k->quantity = 1;
    k->sprite = (cJSON_GetObjectItem(key, "sprite")->valuestring)[0];

    createKey(k);

    return k->itemBase;
}

void createKey(Key* k){
    k->itemBase = malloc(sizeof(ItemBase));
    k->itemBase->x = &(k->x);
    k->itemBase->y = &(k->y);
    k->itemBase->z = &(k->z);
    k->itemBase->quantity = &(k->quantity);
    k->itemBase->sprite = &(k->sprite);

    k->itemBase->object = k;
    k->itemBase->objectType = TYPE_KEY;

    k->itemBase->pickUp = &defaultItemPickup;
    k->itemBase->drop = &defaultItemDrop;
    k->itemBase->render = &defaultItemRender;
    k->itemBase->update = &defaultItemUpdate;
    k->itemBase->isEqual = &isKeyEqual;
    k->itemBase->deleteObject = &deleteKey;
}


int isKeyEqual(Key* k1, Key* k2){
    return((k1->broken == k2->broken) || ((k1->openProb == k2->openProb) && (!strcmp(k1->name, k2->name))));
}
void deleteKey(Key* k){
    free(k->name);
    free(k->itemBase);
    free(k);
}