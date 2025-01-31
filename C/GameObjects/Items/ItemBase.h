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

    int x, y, z;
    wchar_t sprite;
    int id, relId;
    char primaryKey, secondaryKey;
    char* primaryUseName, *secondaryUseName;
    char* name;
    char *type, *subType;
    int collider;
    short color[3];

    int quantity;

    int damage, range;

    int health;

    LinkedList effects;
    int goodness, decayTime, cursed, decayed;

    float openingProb;

    int password, passwordValid, lockBroken;

    int value;

    int fake;
    int locked;
    int hidden;
    int inInventory;
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