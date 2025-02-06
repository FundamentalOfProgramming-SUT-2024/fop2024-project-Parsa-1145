#ifndef PLAYER
#define PLAYER

#include "../GameObjects/Items/Action.h"
#include "../Utilities/LinkedList.h"
#include "./Items/ItemBase.h"

typedef struct{
    int x, y, z;
    int visionRadius;
    int color[3];

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

    float baseLuck;
    float luckModifier;
    float luck;


    int totalGold;
    int totalMoves;

    int levitating;
    int invisible;

    int timeSinceInEnhancedRoom;

    LinkedList items;
    LinkedList effects;
    ItemBase* heldObject;
    ItemBase* equipedAmmo;
} Player;

int getPlayerSpeed(Player* player);
int getPlayerStrength(Player* player);
int getPlayerHealthRegen(Player* player);
void updatePlayerStats(Player* player);
void checkEquiped();
void addPlayerEffect(Effect* e);
void resetPlayerStats(Player* player);
cJSON* savePlayerToJson(Player* p);
Player* loadPlayerFromJson(cJSON* data);
#endif