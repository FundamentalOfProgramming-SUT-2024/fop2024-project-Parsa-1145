#ifndef USEABLE
#define USEABLE
#include "ItemBase.h"
#include "../../Utilities/cJSON.h"

void initConsumable(ItemBase* o);
void initWeapon(ItemBase* o);
void initAmmo(ItemBase* o);
void initKey(ItemBase* o);
void initValueable(ItemBase* o);

void defaultUseableInit(ItemBase* o);

void updateFood(ItemBase* o);

int isFoodEqual(ItemBase* o1, ItemBase* o2);
int isWeaponEqual(ItemBase* w1, ItemBase* w2);

void openKeyInfo(ItemBase* o);
void openPotionInfo(ItemBase* o);
void openFoodInfo(ItemBase* o);
void openAmmoInfo(ItemBase* o);
void openWeaponInfo(ItemBase* o);
void openValueableInfo(ItemBase* o);

void pickupValuable(ItemBase* o);
void dropValueable(ItemBase* o);


void equipInHand(ItemBase* o);

#endif