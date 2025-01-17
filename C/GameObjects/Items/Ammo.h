#ifndef AMMO
#define AMMO

#include "ItemBase.h"
#include "../Camera.h"
#include "../Texture.h"
#include "../../Utilities/cJSON.h"


typedef struct{
    ItemBase* gameObject;

    int x, y, z;
    wchar_t sprite;
    int color;
    char* name;

    int quantity;

    int type;
    float damage;
    float range;
    int ammoType;
}Ammo;

ItemBase* generateAmmo(cJSON* data);
void createAmmo(Ammo* a);
void pickUpAmmo(Ammo* a);
void dropAmmo(Ammo* a);
void renderAmmo(Ammo* a, CharTexture* frameBuffer, ColorTexture* colorBuffer,  Camera* cam);
void updateAmmo(Ammo* a);
void deleteAmmo(ItemBase* a);
void openAmmoInfo(ItemBase* o);
int isAmmoEqual(Ammo* a1, Ammo* a2);

#endif