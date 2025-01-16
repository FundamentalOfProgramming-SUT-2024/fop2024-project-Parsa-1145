#ifndef POTION
#define POTION
#include "ItemBase.h"
#include "../../Utilities/cJSON.h"

typedef struct{
    ItemBase* gameObject;

    int x, y, z;
    wchar_t sprite;
    int color;
    char* name;

    int quantity;
}Potion;

ItemBase* generatePotion(cJSON* data);
void createPotion(Potion* w);
void deletePotion(ItemBase* o);
void pickupPotion(ItemBase* o);
void dropPotion(ItemBase* o);
int isPotionEqual(Potion* w1, Potion* w2);
void usePotion(ItemBase* o);

#endif