#include<string.h>

#include "Useable.h"
#include "../../ProgramStates/MainGame.h"
#include "../../Globals.h"
#include "ItemTypes.h"
#include "Action.h"

void initWeapon(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &pickableItemUpdate;
    o->render = &defaultItemRender;
    o->openItemInfo = &openWeaponInfo;
    o->pickUp = &defaultItemPickup;
    o->drop = &defaultItemDrop;
    o->isEqual = &isWeaponEqual;
}
void initConsumable(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    if(!strcmp(o->subType, "food")){
        o->update = &updateFood;
        o->isEqual = &isFoodEqual;
        o->openItemInfo = &openFoodInfo;
    }else if(!strcmp(o->subType, "potion")){
        o->update = &pickableItemUpdate;
        o->isEqual = &defaultItemCompare;
        o->openItemInfo = &openPotionInfo;
    }
    o->render = &defaultItemRender;
    o->pickUp = &defaultItemPickup;
    o->drop = &defaultItemDrop;
}
void initAmmo(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &pickableItemUpdate;
    o->render = &defaultItemRender;
    o->openItemInfo = &openAmmoInfo;
    o->pickUp = &defaultItemPickup;
    o->drop = &defaultItemDrop;
    o->isEqual = &defaultItemCompare;
}
void initKey(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &pickableItemUpdate;
    o->render = &defaultItemRender;
    o->openItemInfo = &openKeyInfo;
    o->pickUp = &defaultItemPickup;
    o->drop = &defaultItemDrop;
    o->isEqual = &defaultItemCompare;
}
void defaultUseableInit(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &pickableItemUpdate;
    o->render = &defaultItemRender;
    o->openItemInfo = NULL;
    o->pickUp = &defaultItemPickup;
    o->drop = &defaultItemDrop;
    o->isEqual = &defaultItemCompare;
}
void initValueable(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &pickableItemUpdate;
    o->render = &defaultItemRender;
    o->openItemInfo = openValueableInfo;
    o->pickUp = &pickupValuable;
    o->drop = &dropValueable;
    o->isEqual = &defaultItemCompare;
}
void initAmulet(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &pickableItemUpdate;
    o->render = &defaultItemRender;
    o->openItemInfo = openValueableInfo;
    o->pickUp = &pickupAmulet;
    o->drop = &dropValueable;
    o->isEqual = &defaultItemCompare;
}
void openWeaponInfo(ItemBase* o){
    TextWidget* name = malloc(sizeof(TextWidget));
    TextWidget* quantity = malloc(sizeof(TextWidget));
    TextWidget* dmg = malloc(sizeof(TextWidget));

    Button* drop = malloc(sizeof(Button));
    Button* equip = malloc(sizeof(Button));

    emptyWidget(&mgItemWidget);

    createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %S(%u)", o->name, &(o->sprite));
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "quantity: %d", &(o->quantity));
    createTextWidget(dmg, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "damage: %d", &(o->damage));
    createButton    (drop, &mgItemWidget, "drop", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);
    createButton    (equip, &mgItemWidget, "equip", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 5);

    drop->contextObject = o;
    drop->contextCallback = o->drop;
    equip->contextObject = o;
    equip->contextCallback = &equipInHand;

    linkedListPushBack(mgItemWidget.children, name->uiBase);
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);
    linkedListPushBack(mgItemWidget.children, dmg->uiBase);
    if(o->range){
        TextWidget* range = malloc(sizeof(TextWidget));
        createTextWidget(range, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "range: %d", &(o->damage));
        linkedListPushBack(mgItemWidget.children, range->uiBase);
    }
    linkedListPushBack(mgItemWidget.children, drop->uiBase);
    linkedListPushBack(mgItemWidget.children, equip->uiBase);
    shownItem = o;
}
void openFoodInfo(ItemBase* o){
    TextWidget* name = malloc(sizeof(TextWidget));
    TextWidget* quantity = malloc(sizeof(TextWidget));
    TextWidget* goodness = malloc(sizeof(TextWidget));

    Button* drop = malloc(sizeof(Button));
    Button* consume = malloc(sizeof(Button));

    emptyWidget(&mgItemWidget);

    if(!o->goodness){
        createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %S(%U) (rotten)", o->name, &(o->sprite));
    }else{
        createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %S(%U)", o->name, &(o->sprite));
    }
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "quantity: %d", &(o->quantity));
    createTextWidget(goodness, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "goodness: %d", &(o->goodness));
    createButton    (drop, &mgItemWidget, "drop", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);
    createButton    (consume, &mgItemWidget, "consume", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 7);

    drop->contextObject = o;
    drop->contextCallback = o->drop;
    consume->contextObject = o;
    consume->contextCallback = o->primaryUse;

    linkedListPushBack(mgItemWidget.children, name->uiBase);
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);
    linkedListPushBack(mgItemWidget.children, goodness->uiBase);
    linkedListPushBack(mgItemWidget.children, drop->uiBase);
    linkedListPushBack(mgItemWidget.children, consume->uiBase);
    shownItem = o;
}
void openKeyInfo(ItemBase* o){
    TextWidget* name = malloc(sizeof(TextWidget));
    TextWidget* quantity = malloc(sizeof(TextWidget));
    TextWidget* openProb = malloc(sizeof(TextWidget));

    Button* drop = malloc(sizeof(Button));
    Button* equip = malloc(sizeof(Button));

    emptyWidget(&mgItemWidget);

    createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %S(%U)", o->name, &(o->sprite));
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "quantity: %d", &(o->quantity));
    createTextWidget(openProb, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "opening chance: %f", &(o->openingProb));
    createButton    (drop, &mgItemWidget, "drop", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);
    createButton    (equip, &mgItemWidget, "consume", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 7);

    drop->contextObject = o;
    drop->contextCallback = o->drop;
    equip->contextObject = o;
    equip->contextCallback = &equipInHand;

    linkedListPushBack(mgItemWidget.children, name->uiBase);
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);
    linkedListPushBack(mgItemWidget.children, openProb->uiBase);
    linkedListPushBack(mgItemWidget.children, drop->uiBase);
    linkedListPushBack(mgItemWidget.children, equip->uiBase);
    shownItem = o;
}
void openPotionInfo(ItemBase* o){
    TextWidget* name = malloc(sizeof(TextWidget));
    TextWidget* quantity = malloc(sizeof(TextWidget));
    TextWidget* sprite = malloc(sizeof(TextWidget));

    Button* drop = malloc(sizeof(Button));
    Button* consume = malloc(sizeof(Button));

    emptyWidget(&mgItemWidget);

    createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %S", o->name);
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "quantity: %d", &(o->quantity));
    createTextWidget(sprite, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "Shape: %U", &(o->sprite));
    createButton    (drop, &mgItemWidget, "drop", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);
    createButton    (consume, &mgItemWidget, "consume", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 7);

    drop->contextObject = o;
    drop->contextCallback = o->drop;
    consume->contextObject = o;
    consume->contextCallback = o->primaryUse;

    linkedListPushBack(mgItemWidget.children, name->uiBase);
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);
    linkedListPushBack(mgItemWidget.children, sprite->uiBase);
    linkedListPushBack(mgItemWidget.children, drop->uiBase);
    linkedListPushBack(mgItemWidget.children, consume->uiBase);
    shownItem = o;
}
void openAmmoInfo(ItemBase* o){
    TextWidget* name = malloc(sizeof(TextWidget));
    TextWidget* quantity = malloc(sizeof(TextWidget));
    TextWidget* dmg = malloc(sizeof(TextWidget));
    TextWidget* range = malloc(sizeof(TextWidget));

    Button* drop = malloc(sizeof(Button));
    Button* equip = malloc(sizeof(Button));

    emptyWidget(&mgItemWidget);

    createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %S(%U)", o->name, &(o->sprite));
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "quantity: %d", &(o->quantity));
    createTextWidget(dmg, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "damage: %d", &(o->damage));
    createTextWidget(range, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "range: %d", &(o->range));

    createButton(drop, &mgItemWidget, "drop", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);
    createButton(equip, &mgItemWidget, "equip", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 5);

    drop->contextObject = o;
    drop->contextCallback = o->drop;

    if(!strcmp(o->subType, "arrow")){
        equip->contextCallback = &equipArrow;
        equip->contextObject = o;
    }
    
    linkedListPushBack(mgItemWidget.children, name->uiBase);
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);
    linkedListPushBack(mgItemWidget.children, dmg->uiBase);
    linkedListPushBack(mgItemWidget.children, range->uiBase);
    linkedListPushBack(mgItemWidget.children, drop->uiBase);
    linkedListPushBack(mgItemWidget.children, equip->uiBase);
    shownItem = o;
}
void openValueableInfo(ItemBase* o){
    TextWidget* name = malloc(sizeof(TextWidget));
    TextWidget* quantity = malloc(sizeof(TextWidget));
    TextWidget* value = malloc(sizeof(TextWidget));

    Button* drop = malloc(sizeof(Button));
    Button* equip = malloc(sizeof(Button));

    emptyWidget(&mgItemWidget);

    createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %S(%U)", o->name, &(o->sprite));
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "quantity: %d", &(o->quantity));
    createTextWidget(value, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "value: %d", &(o->value));

    createButton(drop, &mgItemWidget, "drop", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);
    createButton(equip, &mgItemWidget, "equip", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 5);

    drop->contextObject = o;
    drop->contextCallback = o->drop;

    linkedListPushBack(mgItemWidget.children, name->uiBase);
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);
    linkedListPushBack(mgItemWidget.children, value->uiBase);
    linkedListPushBack(mgItemWidget.children, drop->uiBase);
    linkedListPushBack(mgItemWidget.children, equip->uiBase);
    shownItem = o;
}

int isConsumableEqual(ItemBase* o1, ItemBase* o2){
    if(!strcmp(o1->name, o2->name)){
        return 1;
    }else return 0;
}
int isWeaponEqual(ItemBase* o1, ItemBase* o2){
    return (!strcmp(o1->name, o2->name));
}
int isFoodEqual(ItemBase* o1, ItemBase* o2){
    return(!strcmp(o1->name, o2->name)) && (o1->decayed == o2->decayed) && (o1->goodness == o2->goodness);
}

void updateFood(ItemBase* o){
    pickableItemUpdate(o);
    if(deltaTime){
        if(o->inInventory){
            o->decayed += 2;
        }else{
            o->decayed += 1;
        }
        if(o->decayed >= o->decayTime){
            o->goodness--;
            o->decayed = 0;
        }
    }
}

void equipInHand(ItemBase* o){
    player.heldObject = o;
    checkEquiped();
}

void pickupValuable(ItemBase* o){
    player.totalGold += o->quantity * o->value;

    defaultItemPickup(o);
}
void dropValueable(ItemBase* o){
    player.totalGold -= o->quantity * o->value;
    defaultItemDrop(o);
    if(o->inInventory) player.totalGold += o->quantity * o->value;
}
void pickupAmulet(ItemBase* o){
    addFormattedMessage("You found the %oamulet of yendor%O", 5, 5, 0);
    endGame(1, NULL);
}

void equipArrow(ItemBase* o){
    player.equipedAmmo = o;
    addFormattedMessage("You equiped %o%S%O", o->color[0], o->color[1], o->color[2], o->name);
}