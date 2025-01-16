#ifndef PLAYER
#define PLAYER

#include "../Utilities/LinkedList.h"
#include "./Items/Weapon.h"

typedef struct{
    int x, y, z;
    int visionRadius;

    int health;
    int maxHealth;
    int strenght;

    int totalGold;

    LinkedList items;
    Weapon* weapon;
} Player;
#endif