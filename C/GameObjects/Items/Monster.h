#ifndef MONSTER
#define MONSTER
#include "ItemBase.h"
#include "../Floor.h"

int defaultMonsterUpdate(ItemBase* m);
int defaultMonsterRender(ItemBase* m, CharTexture* frameBuffer, Camera* cam);
#endif