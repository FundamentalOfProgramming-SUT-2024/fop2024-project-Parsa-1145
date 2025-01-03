#ifndef AMMO
#define AMMO

#include "ItemBase.h"
#include "../Camera.h"
#include "../Texture.h"


typedef struct{
    ItemBase* gameObject;

    int x, y, z;
    char sprite;
    int color;

    int quantity;

    int type;
    float damage;
    float range;
    int level;
    int ammoType;
}Ammo;

void createAmmo(Ammo* a);
void pickUpAmmo(Ammo* a);
void dropAmmo(Ammo* a);
void renderAmmo(Ammo* a, CharTexture* frameBuffer, ColorTexture* colorBuffer,  Camera* cam);
void updateAmmo(Ammo* a);
void deleteAmmo(Ammo* a);
int isAmmoEqual(Ammo* a1, Ammo* a2);

#endif