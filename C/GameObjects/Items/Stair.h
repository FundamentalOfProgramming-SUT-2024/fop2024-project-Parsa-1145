#ifndef STAIR
#define STAIR
#include "ItemBase.h"
#include "../Camera.h"
#include "../Texture.h"

typedef struct Stair{
    ItemBase* itemBase;

    int x, y, z;
    wchar_t sprite;
    int color;

    struct Stair *dest;
}Stair;

void createStair(Stair* d);
void updateStair(ItemBase* o);
void deleteStair(ItemBase* o);


#endif