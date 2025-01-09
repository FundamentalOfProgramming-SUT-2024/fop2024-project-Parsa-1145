#ifndef KEY
#define KEY

#include "ItemBase.h"
#include "../../Utilities/cJSON.h"

typedef struct{
    ItemBase* itemBase;
    char* name;

    int x, y, z;
    wchar_t sprite;
    int color;
    int quantity;
    
    int openProb;
    int broken;
}Key;

ItemBase* generateKey(cJSON* data);
int isKeyEqual(Key* k1, Key* k2);
void createKey(Key* k);
void deleteKey(Key* k);
#endif