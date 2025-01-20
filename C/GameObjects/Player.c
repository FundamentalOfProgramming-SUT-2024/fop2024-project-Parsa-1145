#include "Player.h"

int getPlayerSpeed(Player* player){
    return player->speed * player->speedModifier;
}
int getPlayerStrength(Player* player){
    return player->strenght * player->strengthModifier;
}
int getPlayerHealthRegen(Player* player){
    return player->healthRegen * player->healthRegenModifier;
}