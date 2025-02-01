#ifndef MONSTER
#define MONSTER
#include "ItemBase.h"
#include "../Floor.h"

void initMonster(ItemBase* m);
int defaultMonsterUpdate(ItemBase* m);
int defaultMonsterRender(ItemBase* m, CharTexture* frameBuffer, Camera* cam);
void defaultMonsterAttack(ItemBase* m);
#endif