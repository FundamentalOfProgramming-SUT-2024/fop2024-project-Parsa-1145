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