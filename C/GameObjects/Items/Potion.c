#include<string.h>
#include "Potion.h"
#include "../../ProgramStates/MainGame.h"
#include "../../Globals.h"
#include "ItemTypes.h"

ItemBase* generatePotion(cJSON* data){
    cJSON* potion = cJSON_GetArrayItem(data, chooseWithWeight(data));

    Potion* p = malloc(sizeof(Potion));

    p->quantity = 1;
    p->name = copyString(cJSON_GetObjectItem(potion, "name")->valuestring);
    p->sprite = (cJSON_GetObjectItem(potion, "sprite")->valuestring)[0];

    createPotion(p);

    return p->gameObject;
}
void createPotion(Potion* p){
    p->gameObject = malloc(sizeof(ItemBase));

    p->gameObject->x = &(p->x);
    p->gameObject->y = &(p->y);
    p->gameObject->z = &(p->z);
    p->gameObject->sprite = &(p->sprite);
    p->gameObject->quantity = &(p->quantity);



    p->gameObject->object = p;
    p->gameObject->objectType = TYPE_POTION;

    p->gameObject->render = &defaultItemRender;
    p->gameObject->drop = &defaultItemDrop;
    p->gameObject->pickUp = &defaultItemPickup;
    p->gameObject->update = &defaultItemUpdate;
    p->gameObject->deleteObject = &deletePotion;
    p->gameObject->isEqual = &isPotionEqual;
}
void deletePotion(Potion* p){
    free(p->name);
    free(p->gameObject);
    free(p);
}
int isPotionEqual(Potion* p1, Potion* p2){
    if(!strcmp(p1->name, p2->name)){
        return 1;
    }
}