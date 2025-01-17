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
    p->sprite = (cJSON_GetObjectItem(potion, "sprite")->valueint);

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
    p->gameObject->name = p->name;

    p->gameObject->object = p;
    p->gameObject->objectType = TYPE_POTION;

    p->gameObject->render = &defaultItemRender;
    p->gameObject->drop = &dropPotion;
    p->gameObject->pickUp = &pickupPotion;
    p->gameObject->update = &defaultItemUpdate;
    p->gameObject->deleteObject = &deletePotion;
    p->gameObject->isEqual = &isPotionEqual;
    p->gameObject->openItemInfo = &openPotionInfo;
    
}
void deletePotion(ItemBase* o){
    Potion* p = o->object;
    free(p->name);
    free(p->gameObject);
    free(p);
}
int isPotionEqual(Potion* p1, Potion* p2){
    if(!strcmp(p1->name, p2->name)){
        return 1;
    }else return 0;
}
void pickupPotion(ItemBase* o){
    Potion* p = o->object;

    defaultItemPickup(o);
}
void usePotion(ItemBase* o){
    addMessage(writeLog("You used a %s", o->name));
}

void dropPotion(ItemBase* o){
    defaultItemDrop(o);
}
void openPotionInfo(ItemBase* o){
    Potion* w = o->object;
    TextWidget* name = malloc(sizeof(TextWidget));
    TextWidget* quantity = malloc(sizeof(TextWidget));
    TextWidget* sprite = malloc(sizeof(TextWidget));

    Button* drop = malloc(sizeof(Button));
    Button* equip = malloc(sizeof(Button));

    emptyWidget(&mgItemWidget);

    createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %s", 's', o->name);
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "quantity: %d", 'd', o->quantity);
    createTextWidget(sprite, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "Shape: %u", 's', o->sprite);

    createButton(drop, &mgItemWidget, "drop", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);
    createButton(equip, &mgItemWidget, "consume", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);

    linkedListPushBack(mgItemWidget.children, name->uiBase);
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);
    linkedListPushBack(mgItemWidget.children, sprite->uiBase);
    linkedListPushBack(mgItemWidget.children, drop->uiBase);
    linkedListPushBack(mgItemWidget.children, equip->uiBase);
}