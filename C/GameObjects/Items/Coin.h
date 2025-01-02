#ifndef COIN
#define COIN

#include"ItemBase.h"

typedef struct{
    ItemBase* itmeBase;

    int x, y, z;
    char sprite;
    int color;
    int quantity;

    int coinType;
}Coin;

#endif