#include "Player.h"

#include "../ProgramStates/MainGame.h"

int getPlayerSpeed(Player* player){
    return player->speed * player->speedModifier;
}
int getPlayerStrength(Player* player){
    return player->strenght * player->strengthModifier;
}
int getPlayerHealthRegen(Player* player){
    return player->healthRegen * player->healthRegenModifier;
}
void checkEquiped(){
    if(player.heldObject && !player.heldObject->inInventory){
        changeTextWidget(&mgEquipedNameTextWidget, "Nothing equiped");
        mgEquipedPrimaryTextWidget.widget->isVisible = 0;
        mgEquipedSecondaryextWidget.widget->isVisible = 0;
    }
}