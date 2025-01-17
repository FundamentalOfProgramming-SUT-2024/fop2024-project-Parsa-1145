#include <stdlib.h>
#include <string.h>

#include "Coin.h"
#include "ItemBase.h"
#include "../../ProgramStates/MainGame.h"
#include "../../Globals.h"
#include "ItemTypes.h"

ItemBase* generateCoin(cJSON* data){
    cJSON* coin = cJSON_GetArrayItem(data, chooseWithWeight(data));

    Coin* c = malloc(sizeof(Coin));

    c->name = copyString(cJSON_GetObjectItem(coin, "name")->valuestring);
    c->quantity = randBetween(cJSON_GetObjectItem(coin, "minQuantity")->valuedouble, cJSON_GetObjectItem(coin, "maxQuantity")->valuedouble, 0);
    c->sprite = (cJSON_GetObjectItem(coin, "sprite")->valueint);
    c->value = cJSON_GetObjectItem(coin, "value")->valueint;

    createCoin(c);

    return c->itemBase;
}
void createCoin(Coin* c){
    c->itemBase = malloc(sizeof(ItemBase));
    c->itemBase->x = &(c->x);
    c->itemBase->y = &(c->y);
    c->itemBase->z = &(c->z);
    c->itemBase->quantity = &(c->quantity);
    c->itemBase->sprite = &(c->sprite);
    c->itemBase->name = c->name;


    c->itemBase->object = c;
    c->itemBase->objectType = TYPE_COIN;

    c->itemBase->pickUp = &pickupCoin;
    c->itemBase->drop = &defaultItemDrop;
    c->itemBase->render = &defaultItemRender;
    c->itemBase->update = &defaultItemUpdate;
    c->itemBase->isEqual = &isCoinEqual;
    c->itemBase->deleteObject = &deleteCoin;
    c->itemBase->openItemInfo = &openCoinInfo;
}
void pickupCoin(ItemBase* o){
    Coin* c = o->object;

    player.totalGold += c->quantity * c->value;

    defaultItemPickup(o);
}
int isCoinEqual(Coin* c1, Coin* c2){
    return (!strcmp(c1->name, c2->name));
}
void deleteCoin(ItemBase* o){
    Coin* c = o->object;
    free(c->name);
    free(c->itemBase);
    free(c);
}
void openCoinInfo(ItemBase* o){
    Coin* c = o->object;
    TextWidget* name = malloc(sizeof(TextWidget));
    TextWidget* quantity = malloc(sizeof(TextWidget));
    TextWidget* sprite = malloc(sizeof(TextWidget));

    Button* drop = malloc(sizeof(Button));

    emptyWidget(&mgItemWidget);

    createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %s", o->name);
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "quantity: %d", o->quantity);
    createTextWidget(sprite, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "sprite: %u", o->sprite);

    createButton(drop, &mgItemWidget, "drop", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);

    drop->contextObject = o;
    drop->contextCallback = &defaultItemDrop;

    linkedListPushBack(mgItemWidget.children, name->uiBase);
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);
    linkedListPushBack(mgItemWidget.children, sprite->uiBase);
    linkedListPushBack(mgItemWidget.children, drop->uiBase);
}