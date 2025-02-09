#ifndef ACTION
#define ACTION
#include "ItemBase.h"

typedef struct Effect{
    char* type;
    int (*func)(struct Effect*);
    float amount;
    int duration;
    int elapsed;
}Effect;

typedef struct{
    ItemBase* object;
    int (*func)(struct ItemBase*);
    char* name;
    int key;
}Interaction;

void moveInStair(ItemBase* o);

void deleteInteraction(Interaction* a);
void addInteraction(char* name, void (*func)(ItemBase*),  int key, ItemBase* o);

int consume(ItemBase* o);
int swingAttack(ItemBase* o);
int stab(ItemBase* o);
int castSpell(ItemBase* o);
int kick(ItemBase* o);
int throwAttack(ItemBase* o);
int takeAim(ItemBase* o);
int bowTakeAim(ItemBase* o);
int shootArrow(ItemBase* o);
int wandTakeAim(ItemBase* o);
int unlockDoor(ItemBase* o);
int trapSimpleDamage(ItemBase* o);
int trapPoisonDamage(ItemBase* o);
int trapFallToNextFloor(ItemBase* o);
int trapTeleport(ItemBase* o);

int effectSpeedIncrease(Effect* e);
int effectHealthRegen(Effect* e);
int effectStrengthen(Effect* e);
int effectHeal(Effect* e);
int effectFill(Effect* e);
int effectSicken(Effect* e);
int effectMaxHealthIncrease(Effect* e);
int effectPoison(Effect* e);
int effectLevitation(Effect* e);
int effectBurning(Effect* e);
int effectGreatLuck(Effect* e);
int effectInvisibility(Effect* e);
int makeKey(ItemBase* k);

void deleteEffect(Effect* effect);
cJSON* effectToJson(Effect* e);
Effect* loadEffect(cJSON* data);

void(*getAction(char* name))(ItemBase*);
void(*getEffectFunc(char* name))(Effect*);


#endif