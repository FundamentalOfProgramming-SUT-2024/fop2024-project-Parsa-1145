#ifndef ITEMBASE
#define ITEMBASE

#include <stdlib.h>


#include "../Camera.h"
#include "../Texture.h"
#include "../../Utilities/LinkedList.h"
#include "../../Utilities/cJSON.h"

#include "../../UiElements/Button.h"
#include "../../UiElements/TextBox.h"
#include "../../UiElements/Checkbox.h"
#include "../../UiElements/UiBase.h"
#include "../../UiElements/TabWidget.h"
#include "../../UiElements/TextWidget.h"

extern int globalItemIdCounter;

typedef struct ItemBase{
    void (*update)(struct ItemBase*);
    void (*render)(void*, CharTexture*, Camera*);
    int (*isEqual)(void*, void*);
    void (*pickUp)(struct ItemBase*);
    void (*drop)(struct ItemBase*);
    void (*deleteObject)(struct ItemBase*);
    void (*openItemInfo)(struct ItemBase*);
    void (*primaryUse)(struct ItemBase*);
    void (*secondaryUse)(struct ItemBase*);
    void (*playerCollision)(struct ItemBase*);
    void (*keyPress)(struct ItemBase*);

    int id;
    int relId;
    char *type, *subType;
    int damage, range;
    LinkedList effects;
    int goodness, decayTime, cursed;
    int decayed;
    float openingProb;
    int fake;
    int value;
    char primaryKey, secondaryKey;
    char* primaryUseName, *secondaryUseName;
    int x, y, z;
    wchar_t sprite;
    int quantity;
    char* name;
    int collider;
    int locked;
    int inInventory;
    short color[3];
    int password, passwordValid, lockBroken;
}ItemBase;

ItemBase* loadItemFromJson(cJSON* data);
ItemBase* LoadItemWithName(const char* name);
cJSON* itemToJson(ItemBase* o);

void defaultItemPickup(ItemBase* o);

void noPickUp(ItemBase* o);
void addItemToInventory(ItemBase* o);

void defaultItemDrop(ItemBase* o);
void noDrop(ItemBase* o);

void defaultItemRender(ItemBase* o, CharTexture* frameBuffer, Camera* cam);

int defaultItemCompare(ItemBase* o1, ItemBase* o2);

void defaultItemUpdate(ItemBase* o);
int isPlayerNextTo(ItemBase* o);
void useItem(ItemBase* o);
void pickableItemUpdate(ItemBase* o);

void defaultItemDelete(ItemBase* o);

void debugItemInfo(ItemBase* o);
ItemBase* findItemById(int id);


#endif