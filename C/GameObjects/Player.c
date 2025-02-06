#include <string.h>

#include "Player.h"

#include "../ProgramStates/MainGame.h"

int getPlayerSpeed(Player* player){
    return player->speed * player->speedModifier;
}
int getPlayerStrength(Player* player){
    return player->strenght * player->strengthModifier;
}
int getPlayerHealthRegen(Player* player){
    return player->healthRegenTime * player->healthRegenModifier;
}
void checkEquiped(){
    if(!(player.heldObject) || !(player.heldObject->inInventory)){
        player.heldObject = NULL;
        changeTextWidget(&mgEquipedNameTextWidget, "Nothing equiped");
        mgEquipedPrimaryTextWidget.widget->isVisible = 0;
        mgEquipedSecondaryextWidget.widget->isVisible = 0;
    }else{
        changeTextWidget(&mgEquipedNameTextWidget, "%s equiped", player.heldObject->name);
        if(player.heldObject->primaryUse){
            changeTextWidget(&mgEquipedPrimaryTextWidget, "Press e to %s", player.heldObject->primaryUseName);
            mgEquipedPrimaryTextWidget.widget->isVisible = 1;
        }else{
            mgEquipedPrimaryTextWidget.widget->isVisible = 0;
        }
        if(player.heldObject->secondaryUse){
            changeTextWidget(&mgEquipedSecondaryextWidget, "Press q to %s", player.heldObject->secondaryUseName);
            mgEquipedSecondaryextWidget.widget->isVisible = 1;
        }else{
            mgEquipedSecondaryextWidget.widget->isVisible = 0;
        }
    }

    if(player.equipedAmmo && player.equipedAmmo->inInventory){

    }else{
        player.equipedAmmo = NULL;
    }
}
void resetPlayerStats(Player* p){
    p->healthModifier = 1;
    p->healthRegenModifier = 1;
    p->speedModifier = 1;
    p->luckModifier = 1;
    p->strengthModifier = 1;
    p->invisible = 0;
    p->levitating = 0;
}
void updatePlayerStats(Player* player){
    player->healthRegenAmount = player->baseHealthRegenAmount * player->healthRegenModifier;
    player->speed = player->baseSpeed * player->speedModifier;
    player->strenght = player->baseStrength * player->strengthModifier;
    player->maxHealth = player->baseMaxHealth;
    player->luck = player->baseLuck * player->luckModifier;
}
void addPlayerEffect(Effect* e){
    void** tmp = player.effects.data;
    Effect* ptr;

    while(tmp){
        ptr = tmp[1];
        tmp = tmp[0];
        if(!strcmp(e->type, ptr->type)){
            deleteEffect(ptr);
            removeItemFromLinkedList(&(player.effects), ptr);
            break;
        }
    }
    linkedListPushBack(&(player.effects), e);
    e->elapsed = 0;
    updateEffectsTab();
}

cJSON* savePlayerToJson(Player* p){
    cJSON* playerJ = cJSON_CreateObject();


    cJSON_AddNumberToObject(playerJ, "x", player.x);
    cJSON_AddNumberToObject(playerJ, "y", player.y);
    cJSON_AddNumberToObject(playerJ, "z", player.z);

    cJSON_AddNumberToObject(playerJ, "health", player.health);
    cJSON_AddNumberToObject(playerJ, "baseMaxHealth", player.baseMaxHealth);
    cJSON_AddNumberToObject(playerJ, "healthModifier", player.healthModifier);
    cJSON_AddNumberToObject(playerJ, "maxHealth", player.maxHealth);
    cJSON_AddNumberToObject(playerJ, "baseHealthRegenAmount", player.baseHealthRegenAmount);
    cJSON_AddNumberToObject(playerJ, "healthRegenModifier", player.healthRegenModifier);
    cJSON_AddNumberToObject(playerJ, "healthRegenAmount", player.healthRegenAmount);
    cJSON_AddNumberToObject(playerJ, "healthRegenTime", player.healthRegenTime);
    cJSON_AddNumberToObject(playerJ, "healthRegenCounter", player.healthRegenCounter);

    cJSON_AddNumberToObject(playerJ, "baseStrength", player.baseStrength);
    cJSON_AddNumberToObject(playerJ, "strengthModifier", player.strengthModifier);
    cJSON_AddNumberToObject(playerJ, "strenght", player.strenght);

    cJSON_AddNumberToObject(playerJ, "baseSpeed", player.baseSpeed);
    cJSON_AddNumberToObject(playerJ, "speedModifier", player.speedModifier);
    cJSON_AddNumberToObject(playerJ, "speed", player.speed);

    cJSON_AddNumberToObject(playerJ, "baseFullness", player.baseFullness);
    cJSON_AddNumberToObject(playerJ, "fullness", player.fullness);
    cJSON_AddNumberToObject(playerJ, "hungerTime", player.hungerTime);
    cJSON_AddNumberToObject(playerJ, "hungerCounter", player.hungerCounter);

    cJSON_AddNumberToObject(playerJ, "baseLuck", player.baseLuck);
    cJSON_AddNumberToObject(playerJ, "luckModifier", player.luckModifier);
    cJSON_AddNumberToObject(playerJ, "luck", player.luck);

    cJSON_AddNumberToObject(playerJ, "totalGold", player.totalGold);
    cJSON_AddNumberToObject(playerJ, "totalMoves", player.totalMoves);

    cJSON_AddNumberToObject(playerJ, "levitating", player.levitating);
    cJSON_AddNumberToObject(playerJ, "invisible", player.invisible);


    if(player.heldObject){
        cJSON_AddNumberToObject(playerJ, "heldObject", player.heldObject->id);
    }
    if(player.equipedAmmo){
        cJSON_AddNumberToObject(playerJ, "equipedAmmo", player.equipedAmmo->id);
    }

    cJSON* effectsJ = cJSON_CreateArray();
    {    
        void** tmp = player.effects.data;
        Effect* e;
        while(tmp){
            e = tmp[1];
            tmp = tmp[0];
            cJSON_AddItemToArray(effectsJ, effectToJson(e));
        }
    }
    cJSON_AddItemToObject(playerJ, "effects", effectsJ);
    

    cJSON* inventoryJ = cJSON_CreateArray();
    {
        void** tmpPtr = player.items.data;
        ItemBase* ptr;
        while(tmpPtr){
            ptr = tmpPtr[1];
            tmpPtr = tmpPtr[0];
            cJSON_AddItemToArray(inventoryJ, itemToJson(ptr));
        }
    }
    cJSON_AddItemToObject(playerJ, "inventory", inventoryJ);

    return playerJ;
}
Player* loadPlayerFromJson(cJSON* data){
    player.x = cJSON_GetObjectItem(data, "x")->valueint;
    player.y = cJSON_GetObjectItem(data, "y")->valueint;
    player.z = cJSON_GetObjectItem(data, "z")->valueint;

    player.health = cJSON_GetObjectItem(data, "health")->valueint;
    player.baseMaxHealth = cJSON_GetObjectItem(data, "baseMaxHealth")->valueint;
    player.healthModifier = cJSON_GetObjectItem(data, "healthModifier")->valuedouble;
    player.maxHealth = cJSON_GetObjectItem(data, "maxHealth")->valueint;
    player.baseHealthRegenAmount = cJSON_GetObjectItem(data, "baseHealthRegenAmount")->valueint;
    player.healthRegenModifier = cJSON_GetObjectItem(data, "healthRegenModifier")->valuedouble;
    player.healthRegenAmount = cJSON_GetObjectItem(data, "healthRegenAmount")->valueint;
    player.healthRegenTime = cJSON_GetObjectItem(data, "healthRegenTime")->valueint;
    player.healthRegenCounter = cJSON_GetObjectItem(data, "healthRegenCounter")->valueint;

    player.baseStrength = cJSON_GetObjectItem(data, "baseStrength")->valueint;
    player.strengthModifier = cJSON_GetObjectItem(data, "strengthModifier")->valuedouble;
    player.strenght = cJSON_GetObjectItem(data, "strenght")->valueint;

    player.baseSpeed = cJSON_GetObjectItem(data, "baseSpeed")->valueint;
    player.speedModifier = cJSON_GetObjectItem(data, "speedModifier")->valuedouble;
    player.speed = cJSON_GetObjectItem(data, "speed")->valueint;

    player.baseFullness = cJSON_GetObjectItem(data, "baseFullness")->valueint;
    player.fullness = cJSON_GetObjectItem(data, "fullness")->valueint;
    player.hungerTime = cJSON_GetObjectItem(data, "hungerTime")->valueint;
    player.hungerCounter = cJSON_GetObjectItem(data, "hungerCounter")->valueint;

    player.baseLuck = cJSON_GetObjectItem(data, "baseLuck")->valuedouble;
    player.luckModifier = cJSON_GetObjectItem(data, "luckModifier")->valuedouble;
    player.luck = cJSON_GetObjectItem(data, "luck")->valuedouble;

    player.totalGold = cJSON_GetObjectItem(data, "totalGold")->valueint;
    player.totalMoves = cJSON_GetObjectItem(data, "totalMoves")->valueint;

    player.levitating = cJSON_GetObjectItem(data, "levitating")->valueint;
    player.invisible = cJSON_GetObjectItem(data, "invisible")->valueint;

    emptyLinkedList(&(player.effects));
    emptyLinkedList(&(player.items));

    {
        cJSON* tmp = cJSON_GetObjectItem(data, "effects")->child;
        while(tmp){
            linkedListPushBack(&(player.effects), loadEffect(tmp));
            tmp = tmp->next;
        }
    }

    {
        cJSON* tmp = cJSON_GetObjectItem(data, "inventory")->child;
        while(tmp){
            linkedListPushBack(&(player.items), loadItemFromJson(tmp));
            tmp = tmp->next;
        }
    }

    {
        cJSON* tmp = cJSON_GetObjectItem(data, "heldObject");

        if(tmp){
            player.heldObject = findItemById(tmp->valueint);
        }

        tmp = cJSON_GetObjectItem(data, "equipedAmmo");

        if(tmp){
            player.equipedAmmo = findItemById(tmp->valueint);
        }
    }
}