#include<string.h>

#include "Useable.h"
#include "../../ProgramStates/MainGame.h"
#include "../../UiElements/ImageBox.h"
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
    if(!strcmp(o->name, "Broken key")){
        o->openItemInfo = &openBrokenKeyINfo;
    }else{
        o->openItemInfo = &openKeyInfo;
    }
    o->update = &pickableItemUpdate;
    o->render = &defaultItemRender;
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

void openItemInfo(ItemBase* o){
    emptyWidget(&mgItemWidget);

    TextWidget* name = calloc(1, sizeof(TextWidget));
    if((!strcmp(o->subType, "food")) && (!o->goodness)){
        createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %S(%o%U%O)(%orotten%O)", o->name, o->color[0], o->color[1], o->color[2], o->sprite,
        5, 2, 1);
    }else{
        createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %S(%o%U%O)", o->name, o->color[0], o->color[1], o->color[2], o->sprite);
    }
    linkedListPushBack(mgItemWidget.children, name->uiBase);
    
    
    if(o->asciiArt){
        ImageBox* asciiArt = calloc(1, sizeof(ImageBox));
        createImageBox(asciiArt, &mgItemWidget, o->asciiArt->data, rgb[o->color[0]][o->color[1]][o->color[2]], ALIGN_CENTER, WITH_PARENT, 0, 1);
        linkedListPushBack(mgItemWidget.children, asciiArt->uiBase);
    }

    if(o->description){
        TextWidget* description = calloc(1, sizeof(TextWidget));
        createTextWidget(description, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 1, "%odescription:%O %S", 3, 3, 3, o->description);
        linkedListPushBack(mgItemWidget.children, description->uiBase);
    }

    TextWidget* quantity = calloc(1, sizeof(TextWidget));
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 1, "quantity: %d", &(o->quantity));
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);

    if(!strcmp(o->type, "weapon")){
        TextWidget* dmg = calloc(1, sizeof(TextWidget));
        createTextWidget(dmg, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "damage: %d", &(o->damage));
        linkedListPushBack(mgItemWidget.children, dmg->uiBase);
    }

    if((!strcmp(o->type, "ammo")) || (!strcmp(o->subType, "ranged"))){
        TextWidget* range = calloc(1, sizeof(TextWidget));
        createTextWidget(range, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "range: %d", &(o->range));
        linkedListPushBack(mgItemWidget.children, range->uiBase);
    }

    if(!strcmp(o->type, "consumable")){
        if(!strcmp(o->subType, "food")){
            TextWidget* goodness = calloc(1, sizeof(TextWidget));
            createTextWidget(goodness, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "goodness: %d", &(o->goodness));
            linkedListPushBack(mgItemWidget.children, goodness->uiBase);
        }else if(!strcmp(o->subType, "potion")){
            TextWidget* effectLabel = calloc(1, sizeof(TextWidget));
            createTextWidget(effectLabel, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "effect: %S", (((Effect*)((o->effects.data)[1]))->type));
            linkedListPushBack(mgItemWidget.children, effectLabel->uiBase);
        }
    }

    if(o->value){
        TextWidget* value = calloc(1, sizeof(TextWidget));
        createTextWidget(value, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "value: %o%d%O",4, 4, 1 &(o->value));
        linkedListPushBack(mgItemWidget.children, value->uiBase);
    }

    Button* drop = calloc(1, sizeof(Button));
    createButton    (drop, &mgItemWidget, "drop", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 4);
    linkedListPushBack(mgItemWidget.children, drop->uiBase);
    drop->contextObject = o;
    drop->contextCallback = o->drop;

    o->openItemInfo(o);

    shownItem = o;
}
void openWeaponInfo(ItemBase* o){
    Button* equip = calloc(1, sizeof(Button));
    createButton(equip, &mgItemWidget, "equip", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 7);
    equip->contextObject = o;
    equip->contextCallback = &equipInHand;
    linkedListPushBack(mgItemWidget.children, equip->uiBase);
}
void openFoodInfo(ItemBase* o){
    Button* consume = calloc(1, sizeof(Button));
    createButton(consume, &mgItemWidget, "consume", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 7);
    consume->contextObject = o;
    consume->contextCallback = o->primaryUse;
    linkedListPushBack(mgItemWidget.children, consume->uiBase);
}
void openKeyInfo(ItemBase* o){
    Button* equip = calloc(1, sizeof(Button));
    createButton    (equip, &mgItemWidget, "equip", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 7);
    equip->contextObject = o;
    equip->contextCallback = &equipInHand;
    linkedListPushBack(mgItemWidget.children, equip->uiBase);
}
void openPotionInfo(ItemBase* o){
    Button* consume = calloc(1, sizeof(Button));
    createButton(consume, &mgItemWidget, "consume", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 7);
    consume->contextObject = o;
    consume->contextCallback = o->primaryUse;
    linkedListPushBack(mgItemWidget.children, consume->uiBase);
}
void openAmmoInfo(ItemBase* o){
    Button* equip = calloc(1, sizeof(Button));
    createButton(equip, &mgItemWidget, "equip", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 5);
    equip->contextCallback = &equipAmmo;
    equip->contextObject = o;
    linkedListPushBack(mgItemWidget.children, equip->uiBase);
}
void openValueableInfo(ItemBase* o){
}
void openBrokenKeyINfo(ItemBase* o){
    Button* equip = malloc(sizeof(Button));
    createButton    (equip, &mgItemWidget, "combine to make key", ABSOLUTE, ALIGN_LEFT, WITH_PARENT, 0, 1, 21);
    equip->contextObject = o;
    equip->contextCallback = &makeKey;
    linkedListPushBack(mgItemWidget.children, equip->uiBase);
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
            if(o->goodness >= 0){
                o->goodness--;
            }
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

void equipAmmo(ItemBase* o){
    player.equipedAmmo = o;
    addFormattedMessage("You equiped %o%S%O", o->color[0], o->color[1], o->color[2], o->name);
}