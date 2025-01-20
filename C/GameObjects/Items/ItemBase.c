#include <string.h>

#include "ItemBase.h"
#include "../../ProgramStates/MainGame.h"
#include "../../Globals.h"
#include "../../GlobalDefines.h"


#include "Action.h"
#include "Useable.h"
#include "Intractable.h"
#include "../Floor.h"

int globalItemIdCounter = 1;


ItemBase* loadItem(cJSON* data){
    ItemBase* o = calloc(1, sizeof(ItemBase));

    cJSON* object = data->child;
    while(object){
        if(!strcmp(object->string, "name")){
            o->name = copyString(object->valuestring);
        }else if(!strcmp(object->string, "type")){
            o->type = copyString(object->valuestring);
        }else if(!strcmp(object->string, "subType")){
            o->subType = copyString(object->valuestring);
        }else if(!strcmp(object->string, "damage")){
            o->damage = object->valueint;
        }else if(!strcmp(object->string, "sprite")){
            o->sprite = object->valueint;
        }else if(!strcmp(object->string, "primaryUse")){
            o->primaryUse = getAction(object->valuestring);
            o->primaryUseName = copyString(object->valuestring);
        }else if(!strcmp(object->string, "secondaryUse")){
            o->secondaryUse = getAction(object->valuestring);
            o->secondaryUseName = copyString(object->valuestring);
        }else if(!strcmp(object->string, "range")){
            o->range = object->valueint;
        }else if(!strcmp(object->string, "openProb")){
            o->openingProb = object->valuedouble;
        }else if(!strcmp(object->string, "decayTime")){
            o->decayTime = object->valueint;
        }else if(!strcmp(object->string, "goodness")){
            o->goodness = object->valueint;
        }else if(!strcmp(object->string, "cursedProb")){
            o->cursed = randWithProb(object->valuedouble);
        }else if(!strcmp(object->string, "effects")){
            createLinkedList(&(o->effects), sizeof(Effect*));
            cJSON* effects = object->child;
            while(effects){
                Effect* new = malloc(sizeof(Effect));
                if(cJSON_GetObjectItem(effects, "type")){
                    new->type = copyString(cJSON_GetObjectItem(effects, "type")->valuestring);
                    new->func = getEffectFunc(new->type);
                }if(cJSON_GetObjectItem(effects, "amount")){
                    new->amount = cJSON_GetObjectItem(effects, "amount")->valuedouble;
                }if(cJSON_GetObjectItem(effects, "duration")){
                    new->duration = cJSON_GetObjectItem(effects, "duration")->valueint;
                }
                linkedListPushBack(&(o->effects), new);
                effects = effects->next;
            }
        }else if(!strcmp(object->string, "value")){
            o->value = object->valueint;
        }else if(!strcmp(object->string, "collider")){
            o->collider = object->valueint;
        }else if(!strcmp(object->string, "id")){
            o->id = object->valueint;
        }else if(!strcmp(object->string, "inInventory")){
            o->inInventory = object->valueint;
        }else if(!strcmp(object->string, "decayed")){
            o->decayed = object->valueint;
        }
        object = object->next;
    }



    if(!strcmp(o->type, "consumable")){
        initConsumable(o);
    }else if(!strcmp(o->type, "weapon")){
        initWeapon(o);
    }else if(!strcmp(o->type, "useable")){
        if(!strcmp(o->subType, "key")){
            initKey(o);
        }
    }else if(!strcmp(o->type, "ammo")){
        initAmmo(o);
    }else if(!strcmp(o->type, "intractable")){
        if(!strcmp(o->name, "Door")){
            initDoor(o);
        }else if(!strcmp(o->name, "Stair")){
            initStair(o);
        }else if(!strcmp(o->name, "Trap")){
            initTrap(o);
        }else if(!strcmp(o->name, "Password Generator")){
            initPasswordGenerator(o);
        }
    }else{
        defaultUseableInit(o);
    }
    return o;
}
void defaultItemPickup(ItemBase* o){
    removeItemFromLinkedList(floors[player.z].itemList, o);

    addMessage(writeLog("You picked up %d %s", o->quantity,  o->name));

    ItemBase* tmp;
    FOR(i, player.items.size){
        tmp = linkedListGetElement(&player.items, i);
        if(o->isEqual(tmp, o)){
            tmp->quantity += o->quantity;
            defaultItemDelete(o);
            return;
        }
    }
    o->inInventory = 1;
    linkedListPushBack(&(player.items), o);
    updateInventoryTab();
}
void defaultItemDrop(ItemBase* o){
    if(validForItemPosition(player.x, player.y, player.z)){
        o->x = player.x;
        o->y = player.y;
        linkedListPushBack(floors[player.z].itemList, o);
        removeItemFromLinkedList(&(player.items), o);
        removeItemFromLinkedList(&(player.items), o);
        addMessage(writeLog("You dropped up a %s", o->name));
        updateInventoryTab();
        emptyWidget(&mgItemWidget);
        o->inInventory = 0;

        checkEquiped();
    }else{
        addMessage(writeLog("You can not drop here"));
    }
}
void defaultItemDelete(ItemBase* o){
    free(o->type);
    free(o->subType);
    free(o->name);
    free(o->primaryUseName);
    free(o->secondaryUseName);
    free(o);
}
void defaultItemRender(ItemBase* o, CharTexture* frameBuffer, ColorTexture* colorBuffer, Camera* cam){
    if(isinRect(o->x, o->y, cam->x, cam->y, cam->w, cam->h)){
        frameBuffer->data[o->y - cam->y][o->x - cam->x] = o->sprite;
    }
}
int defaultItemCompare(ItemBase* o1, ItemBase* o2){
    return (!strcmp(o1->name, o2->name));
}
ItemBase* findItemById(int id){
    static Floor* f;
    static void** tmpIterPtr;
    static ItemBase* iterPtr;
    FOR(i, floorNum){
        f = floors + i;
        tmpIterPtr = f->itemList->data;
        while(tmpIterPtr){
            iterPtr = tmpIterPtr[1];
            tmpIterPtr = tmpIterPtr[0];
            if(iterPtr->id == id){
                return iterPtr;
            }
        }
    }
    tmpIterPtr = player.items.data;
    while(tmpIterPtr){
        iterPtr = tmpIterPtr[1];
        tmpIterPtr = tmpIterPtr[0];
        if(iterPtr->id == id){
            return iterPtr;
        }
    }
    return NULL;
}
void defaultItemUpdate(ItemBase* o){
    
}
void noPickUp(ItemBase* g){}
void noDrop(ItemBase* g){}

void pickableItemUpdate(ItemBase* o){
    if((!o->inInventory) && (player.x == o->x) && (player.y == o->y)){
        o->pickUp(o);
    }
}
void debugItemInfo(ItemBase* o){
    TextWidget* name = malloc(sizeof(TextWidget));
    TextWidget* quantity = malloc(sizeof(TextWidget));
    TextWidget* id = malloc(sizeof(TextWidget));
    TextWidget* relId = malloc(sizeof(TextWidget));
    TextWidget* type = malloc(sizeof(TextWidget));
    TextWidget* subType = malloc(sizeof(TextWidget));
    TextWidget* damage = malloc(sizeof(TextWidget));
    TextWidget* range = malloc(sizeof(TextWidget));
    TextWidget* goodness = malloc(sizeof(TextWidget));
    TextWidget* decayTime = malloc(sizeof(TextWidget));
    TextWidget* cursed = malloc(sizeof(TextWidget));
    TextWidget* decayed = malloc(sizeof(TextWidget));
    TextWidget* openingProb = malloc(sizeof(TextWidget));
    TextWidget* fake = malloc(sizeof(TextWidget));
    TextWidget* value = malloc(sizeof(TextWidget));
    TextWidget* primaryUseName = malloc(sizeof(TextWidget));
    TextWidget* secondaryUseName = malloc(sizeof(TextWidget));
    TextWidget* collider = malloc(sizeof(TextWidget));
    TextWidget* locked = malloc(sizeof(TextWidget));
    TextWidget* inInventory = malloc(sizeof(TextWidget));

    emptyWidget(&mgItemWidget);

    createTextWidget(name, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "name: %s(%u)", o->name, &(o->sprite));
    createTextWidget(quantity, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "quantity: %d", &(o->quantity));
    createTextWidget(id, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "id: %d", o->name, &(o->id));
    createTextWidget(relId, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "relId: %d", &(o->relId));
    createTextWidget(type, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "type: %s", (o->type));
    createTextWidget(subType, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "subType: %s", (o->subType));
    createTextWidget(damage, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "damage: %d", &(o->damage));
    createTextWidget(range, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "range: %d", &(o->range));
    createTextWidget(goodness, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "goodness: %d", &(o->goodness));
    createTextWidget(decayTime, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "decayTime: %d", &(o->decayTime));
    createTextWidget(cursed, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "cursed: %d", &(o->cursed));
    createTextWidget(decayed, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "decayed: %d", &(o->decayed));
    createTextWidget(openingProb, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "openingProb: %f", &(o->openingProb));
    createTextWidget(fake, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "fake: %d", &(o->fake));
    createTextWidget(value, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "value: %d", &(o->value));
    createTextWidget(primaryUseName, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "primaryUseName: %s", (o->primaryUseName));
    createTextWidget(secondaryUseName, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "secondaryUseName: %s", (o->secondaryUseName));
    createTextWidget(inInventory, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "inInventory: %d", &(o->inInventory));

    linkedListPushBack(mgItemWidget.children, name->uiBase);
    linkedListPushBack(mgItemWidget.children, quantity->uiBase);
    linkedListPushBack(mgItemWidget.children, id->uiBase);
    linkedListPushBack(mgItemWidget.children, relId->uiBase);
    linkedListPushBack(mgItemWidget.children, type->uiBase);
    linkedListPushBack(mgItemWidget.children, subType->uiBase);
    linkedListPushBack(mgItemWidget.children, damage->uiBase);
    linkedListPushBack(mgItemWidget.children, range->uiBase);
    linkedListPushBack(mgItemWidget.children, goodness->uiBase);
    linkedListPushBack(mgItemWidget.children, decayTime->uiBase);
    linkedListPushBack(mgItemWidget.children, cursed->uiBase);
    linkedListPushBack(mgItemWidget.children, decayed->uiBase);
    linkedListPushBack(mgItemWidget.children, openingProb->uiBase);
    linkedListPushBack(mgItemWidget.children, fake->uiBase);
    linkedListPushBack(mgItemWidget.children, value->uiBase);
    linkedListPushBack(mgItemWidget.children, primaryUseName->uiBase);
    linkedListPushBack(mgItemWidget.children, secondaryUseName->uiBase);
    linkedListPushBack(mgItemWidget.children, inInventory->uiBase);
}