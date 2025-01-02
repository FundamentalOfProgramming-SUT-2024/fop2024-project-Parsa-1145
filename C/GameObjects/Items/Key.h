#ifndef KEY
#define KEY

#include "ItemBase.h"

typedef struct{
    ItemBase* itemBase;

    int x, y, z;
    char sprite;
    int color;
    int quantity;
    
    int keyType;
    int breakProb;
}Key;
#endif