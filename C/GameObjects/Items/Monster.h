#ifndef MONSTER
#define MONSTER
#include "ItemBase.h"
#include "../Floor.h"

void initMonster(ItemBase* m);
int defaultMonsterUpdate(ItemBase* m);
int defaultMonsterRender(ItemBase* m, CharTexture* frameBuffer, Camera* cam);
void defaultMonsterAttack(ItemBase* m);
void defaultMonsterDeath(ItemBase* m);
void defaultMonsterTakeDamage(ItemBase* m, ItemBase* o, int damage);

#endif