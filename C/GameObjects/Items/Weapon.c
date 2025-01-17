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
    weapon->gameObject->drop = &dropWeapon;
    weapon->gameObject->pickUp = &pickUpWeapon;
    weapon->gameObject->update = &updateWeapon;
    weapon->gameObject->deleteObject = &deleteWeapon;
    weapon->gameObject->isEqual = &isWeaponEqual;
    weapon->gameObject->openItemInfo = &openWeaponInfo;
}
void pickUpWeapon(ItemBase* o){
    Weapon* w = o->object;
    defaultItemPickup(o);
}
void dropWeapon(ItemBase* o){
    Weapon* w = o->object;
    defaultItemDrop(o);
}
void renderWeapon(Weapon* w, CharTexture* frameBuffer, ColorTexture* colorBuffer, Camera* cam){
    if(isinRect(w->x, w->y, cam->x, cam->y, cam->w, cam->h)){
        frameBuffer->data[w->y - cam->y][w->x - cam->x] = w->sprite;
    }
}
void updateWeapon(ItemBase* o){
    Weapon* w = o->object;
    if((player.x == w->x) && (player.y == w->y)){
        pickUpWeapon(o);
    }
}
void deleteWeapon(ItemBase* o){
    Weapon* w = o->object;
    free(w->name);
    free(w->gameObject);
    free(w);
}

int isWeaponEqual(Weapon* w1, Weapon* w2){
    return (!strcmp(w1->name, w2->name));
}
void openWeaponInfo(ItemBase* o){
    Weapon* w = o->object;
    TextWidget* name = malloc(sizeof(TextWidget));
    TextWidget* quantity = malloc(sizeof(TextWidget));
    TextWidget* dmg = malloc(sizeof(TextWidget));
    TextWidget* range = malloc(sizeof(TextWidget));
    TextWidget* sprite = malloc(sizeof(TextWidget));

    Button* drop = malloc(sizeof(Button));
    Button* equip = malloc(sizeof(Button));

    emptyWidget(&mgItemWidget);

    createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %s", 's', o->name);
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "quantity: %d", 'd', o->quantity);
    createTextWidget(dmg, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "damage: %d", 'd', &(w->damage));
    createTextWidget(range, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "range: %d", 'd', &(w->damage));
    createTextWidget(sprite, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "sprite: %u", 's', o->sprite);

    createButton(drop, &mgItemWidget, "drop", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);
    createButton(equip, &mgItemWidget, "equip", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);

    drop->contextObject = o;
    drop->contextCallback = &defaultItemDrop;

    linkedListPushBack(mgItemWidget.children, name->uiBase);
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);
    linkedListPushBack(mgItemWidget.children, dmg->uiBase);
    linkedListPushBack(mgItemWidget.children, range->uiBase);
    linkedListPushBack(mgItemWidget.children, sprite->uiBase);
    linkedListPushBack(mgItemWidget.children, drop->uiBase);
    linkedListPushBack(mgItemWidget.children, equip->uiBase);
}