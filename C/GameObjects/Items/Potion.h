#ifndef POTION
#define POTION
#include "ItemBase.h"
#include "../../Utilities/cJSON.h"

typedef struct{
    ItemBase* gameObject;

    int x, y, z;
    char sprite;
    int color;
    char* name;

    int quantity;
}Potion;

ItemBase* generatePotion(cJSON* data);
void createPotion(Potion* w);
void deletePotion(Potion* w);
int isPotionEqual(Potion* w1, Potion* w2);

#endif