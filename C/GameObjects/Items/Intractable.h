#ifndef INTRACTABLE
#define INTRACTABLE
#include "ItemBase.h"

void initDoor(ItemBase* o);
void initStair(ItemBase* o);
void initTrap(ItemBase* o);
void initPasswordGenerator(ItemBase* o);

void trapPlayerCollision(ItemBase* o);
void doorPlayerCollision(ItemBase* o);
void stairPlayerCollision(ItemBase* o);
void passwordGeneratorPlayerCollision(ItemBase* o);

void updatePasswordGenerator(ItemBase* o);
void updateDoor(ItemBase* o);

void stairUpdate(ItemBase* o);

void invalidatePassword(ItemBase* o);

#endif