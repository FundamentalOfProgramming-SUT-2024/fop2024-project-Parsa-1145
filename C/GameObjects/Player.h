#ifndef PLAYER
#define PLAYER

#include "../Utilities/LinkedList.h"
#include "./Items/ItemBase.h"

typedef struct{
    int x, y, z;
    int visionRadius;

    LinkedList items;
} Player;

#endif