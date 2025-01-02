#ifndef WEAPON
#define WEAPON

#include "ItemBase.h"
#include "../Camera.h"
#include "../Texture.h"


typedef struct{
    ItemBase* gameObject;

    int x, y, z;
    char sprite;
    int color;

    float damage;
    float range;
    int level;
    int weaponType;
    int quantity;
}Weapon;

void createWeapon(Weapon* w);
void pickUpWeapon(Weapon* w);
void dropWeapon(Weapon* w);
void renderWeapon(Weapon* w, CharTexture* frameBuffer, ColorTexture* colorBuffer,  Camera* cam);
void updateWeapon(Weapon* w);
void deleteWeapon(Weapon* w);
void isWeaponEqual(Weapon* w1, Weapon* w2);

#endif