#ifndef PLAYER
#define PLAYER

#include "../Utilities/LinkedList.h"
#include "./Items/ItemBase.h"

typedef struct{
    int x, y, z;
    int visionRadius;

    int health;
    int maxHealth;
    int healthRegen;

    int strenght;
    int speed;

    float speedModifier;
    float healthModifier;
    float healthRegenModifier;
    float strengthModifier;

    int totalGold;

    LinkedList items;
    LinkedList effects;
    ItemBase* heldObject;
} Player;

int getPlayerSpeed(Player* player);
int getPlayerStrength(Player* player);
int getPlayerHealthRegen(Player* player);
void checkEquiped();
#endif