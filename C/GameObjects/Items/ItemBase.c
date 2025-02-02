#include <string.h>

#include "ItemBase.h"
#include "../../ProgramStates/MainGame.h"
#include "../../Globals.h"
#include "../../GlobalDefines.h"


#include "Action.h"
#include "Useable.h"
#include "Intractable.h"
#include "Monster.h"
#include "../Floor.h"

int globalItemIdCounter = 1;


ItemBase* loadItemFromJson(cJSON* data){
    ItemBase* o = calloc(1, sizeof(ItemBase));

    cJSON* object = data->child;

    o->color[0] = 5;
    o->color[1] = 5;
    o->color[2] = 5;
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
        }else if(!strcmp(object->string, "inHandUpdate")){
            o->inHandUpdate = getAction(object->valuestring);
            o->inHandUpdateName = copyString(object->valuestring);
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
        }else if(!strcmp(object->string, "cursed")){
            o->cursed = object->valueint;
        }else if(!strcmp(object->string, "cr")){
            o->color[0] = object->valueint;
        }else if(!strcmp(object->string, "cg")){
            o->color[1] = object->valueint;
        }else if(!strcmp(object->string, "cb")){
            o->color[2] = object->valueint;
        }else if(!strcmp(object->string, "visionRadius")){
            o->visionRadius = object->valueint;
        }else if(!strcmp(object->string, "health")){
            o->health = object->valueint;
        }else if(!strcmp(object->string, "lockBroken")){
            o->lockBroken = object->valueint;
        }else if(!strcmp(object->string, "locked")){
            o->locked = object->valueint;
        }else if(!strcmp(object->string, "hidden")){
            o->hidden = object->valueint;
        }else if(!strcmp(object->string, "effects")){
            createLinkedList(&(o->effects), sizeof(Effect*));
            cJSON* effects = object->child;
            while(effects){
                linkedListPushBack(&(o->effects), loadEffect(effects));
                effects = effects->next;
            }
        }else if(!strcmp(object->string, "value")){
            o->value = object->valueint;
        }else if(!strcmp(object->string, "collider")){
            o->collider = object->valueint;
        }else if(!strcmp(object->string, "id")){
            o->id = object->valueint;
        }else if(!strcmp(object->string, "relId")){
            o->relId = object->valueint;
        }else if(!strcmp(object->string, "inInventory")){
            o->inInventory = object->valueint;
        }else if(!strcmp(object->string, "decayed")){
            o->decayed = object->valueint;
        }else if(!strcmp(object->string, "x")){
            o->x = object->valueint;
        }else if(!strcmp(object->string, "y")){
            o->y = object->valueint;
        }else if(!strcmp(object->string, "z")){
            o->z = object->valueint;
        }else if(!strcmp(object->string, "quantity")){
            o->quantity = object->valueint;
        }
        object = object->next;
    }

    if(o->type){
        if(!strcmp(o->type, "monster")){
            initMonster(o);
        }
        else if(!strcmp(o->type, "consumable")){
            initConsumable(o);
        }else if(!strcmp(o->type, "weapon")){
            initWeapon(o);
        }else if(!strcmp(o->type, "useable")){
            if(!strcmp(o->subType, "key")){
                initKey(o);
            }
        }else if(!strcmp(o->type, "ammo")){
            initAmmo(o);
        }else if(!strcmp(o->type, "valueable")){
            initValueable(o);
        }else if(!strcmp(o->type, "endgame")){
            initAmulet(o);
        }else{
            defaultUseableInit(o);
        }
    }else {
        if(!strcmp(o->name, "Door")){
            initDoor(o);
        }else if(!strcmp(o->name, "Stair")){
            initStair(o);
        }else if(!strcmp(o->name, "Trap")){
            initTrap(o);
        }else if(!strcmp(o->name, "Password generator")){
            initPasswordGenerator(o);
        }
    }
    return o;
}
cJSON* itemToJson(ItemBase* o){
    cJSON* json = cJSON_CreateObject();

    cJSON_AddStringToObject(json, "name", o->name);
    cJSON_AddStringToObject(json, "type", o->type);
    cJSON_AddStringToObject(json, "subType", o->subType);
    cJSON_AddNumberToObject(json, "damage", o->damage);
    cJSON_AddNumberToObject(json, "sprite", o->sprite);
    cJSON_AddStringToObject(json, "subType", o->subType);
    cJSON_AddStringToObject(json, "primaryUse", o->primaryUseName);
    cJSON_AddStringToObject(json, "secondaryUse", o->secondaryUseName);
    cJSON_AddNumberToObject(json, "range", o->range);
    cJSON_AddNumberToObject(json, "openProb", o->openingProb);
    cJSON_AddNumberToObject(json, "decayTime", o->decayTime);
    cJSON_AddNumberToObject(json, "goodness", o->goodness);
    cJSON_AddNumberToObject(json, "cursed", o->cursed);
    cJSON_AddNumberToObject(json, "cr", o->color[0]);
    cJSON_AddNumberToObject(json, "cg", o->color[1]);
    cJSON_AddNumberToObject(json, "cb", o->color[2]);
    cJSON_AddNumberToObject(json, "lockBroken", o->lockBroken);
    cJSON_AddNumberToObject(json, "quantity", o->quantity);
    cJSON_AddNumberToObject(json, "locked", o->locked);
    cJSON_AddNumberToObject(json, "hidden", o->hidden);
    cJSON_AddNumberToObject(json, "visionRadius", o->visionRadius);
    cJSON_AddNumberToObject(json, "health", o->health);
    cJSON_AddNumberToObject(json, "value", o->value);
    cJSON_AddNumberToObject(json, "collider", o->collider);
    cJSON_AddNumberToObject(json, "id", o->id);
    cJSON_AddNumberToObject(json, "relId", o->relId);
    cJSON_AddNumberToObject(json, "inInventory", o->inInventory);
    cJSON_AddNumberToObject(json, "decayed", o->decayed);
    cJSON_AddNumberToObject(json, "x", o->x);
    cJSON_AddNumberToObject(json, "y", o->y);
    cJSON_AddNumberToObject(json, "z", o->z);
    if(o->effects.size){
        cJSON* tmp = cJSON_CreateArray();
        
        void** tmpPtr = o->effects.data;
        Effect* ptr;
        while(tmpPtr){
            ptr = tmpPtr[1];
            tmpPtr = tmpPtr[0];

            cJSON* tmp2 = effectToJson(ptr);

            cJSON_AddItemToArray(tmp, tmp2);
        }
        cJSON_AddItemToObject(json, "effects", tmp);
    }

    return json;
}
ItemBase* LoadItemWithName(const char* name){
    cJSON* tmp = itemsJson->child;
    int found = 0;
    while(tmp){
        if(!strcmp(cJSON_GetObjectItem(tmp, "name")->valuestring, name)){
            return loadItemFromJson(tmp);
        }else{
            tmp = tmp->next;
        }
    }
    return NULL;
}
void defaultItemPickup(ItemBase* o){
    removeItemFromLinkedList(floors[player.z].itemList, o);
    if(!o->fake){
        if(o->quantity == 1){
            if(isVowel(o->name[0])){
                addMessage(writeLog("You picked up an %s", o->name));
            }else{
                addMessage(writeLog("You picked up a %s", o->name));
            }
        }else{
            addMessage(writeLog("You picked up %d %ss", o->quantity, o->name));
        }
        addItemToInventory(o);
    }else{
        addMessage(writeLog("You are hallucinating", o->quantity, o->name));
        defaultItemDelete(o);
    }

}
void addItemToInventory(ItemBase* o){
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
        if(o->quantity == 1){
            if(isVowel(o->name[0])){
                addMessage(writeLog("You dropped an %s", o->name));
            }else{
                addMessage(writeLog("You dropped a %s", o->name));
            }
        }else{
            addMessage(writeLog("You dropped %d %ss", o->quantity, o->name));
        }
        updateInventoryTab();
        emptyWidget(&mgItemWidget);
        o->inInventory = 0;

        checkEquiped();
        updateWorld(0, 0);
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
void defaultItemRender(ItemBase* o, CharTexture* frameBuffer, Camera* cam){
    if(((!o->hidden) || gameSettings.debugSeeAll) && isinRect(o->x, o->y, cam->x, cam->y, cam->w, cam->h)){
        if(frameBuffer->depth[o->y - cam->y][o->x - cam->x] <= 1){
            frameBuffer->data[o->y - cam->y][o->x - cam->x] = o->sprite;
        }
        if(frameBuffer->colorDepth[o->y - cam->y][o->x - cam->x] <= 1){
            if(o->color[0] != -1){
                frameBuffer->color[o->y - cam->y][o->x - cam->x] = rgb[o->color[0]][o->color[1]][o->color[2]];
            }
        }
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
        addInteraction(writeLog("pickup %s", o->name), o->pickUp, 'g', o);
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
    createTextWidget(id, &mgItemWidget, ALIGN_LEFT, WITH_PARENT, 0, 0, "id: %d", &(o->id));
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
    shownItem = o;
}

int isPlayerNextTo(ItemBase* o){
    int x = player.x - o->x, y = player.y - o->y;
    return x * x + y * y < 2;
}
void useItem(ItemBase* o){
    if(o->quantity > 1){
        o->quantity--;
    }
    else{
        if(player.heldObject == o){
            player.heldObject = NULL;
        }
        if(shownItem == o){
            emptyWidget(&mgItemWidget);
        }
        checkEquiped();
        removeItemFromLinkedList(&(player.items), o);
        defaultItemDelete(o);
        renderMainGame();
    }
    updateInventoryTab();
}