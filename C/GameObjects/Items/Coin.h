#ifndef COIN
#define COIN

#include"ItemBase.h"
#include "../../Utilities/cJSON.h"


typedef struct{
    ItemBase* itemBase;
    char* name;

    int x, y, z;
    wchar_t sprite;
    int color;
    int quantity;

    int value;
}Coin;


ItemBase* generateCoin(cJSON* data);
void createCoin(Coin* c);
void pickupCoin(ItemBase* o);
int isCoinEqual(Coin* c1, Coin* c2);
void deleteCoin(ItemBase* o);
void openCoinInfo(ItemBase* o);
#endif