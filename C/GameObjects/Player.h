#ifndef PLAYER
#define PLAYER

#include "../GameObjects/Items/Action.h"
#include "../Utilities/LinkedList.h"
#include "./Items/ItemBase.h"

typedef struct{
    int x, y, z;
    int visionRadius;

    int health;

    int baseMaxHealth;
    float healthModifier;
    int maxHealth;

    int baseHealthRegenAmount;
    float healthRegenModifier;
    int healthRegenAmount;

    int baseStrength;
    float strengthModifier;
    int strenght;

    int baseSpeed;
    float speedModifier;
    int speed;

    int healthRegenTime;
    int healthRegenCounter;

    int baseFullness;
    int fullness;
    int hungerTime;
    int hungerCounter;


    int totalGold;
    int totalMoves;

    int levitating;

    LinkedList items;
    LinkedList effects;
    ItemBase* heldObject;
} Player;

int getPlayerSpeed(Player* player);
int getPlayerStrength(Player* player);
int getPlayerHealthRegen(Player* player);
void updatePlayerStats(Player* player);
void checkEquiped();
void addPlayerEffect(Effect* e);
#endif