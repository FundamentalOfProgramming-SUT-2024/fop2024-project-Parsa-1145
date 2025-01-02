#ifndef WEAPON
#define WEAPON

#include "GameObject.h"
#include "Camera.h"
#include "Texture.h"


typedef struct{
    GameObject* gameObject;

    int type;
    int x, y, z;
    char sprite;
    int color;

    float damage;
    float range;
    int level;
}Weapon;

void createWeapon(Weapon* weapon);
void pickUpWeapon(void* weapon);
void dropWeapon(void* weapon);
void renderWeapon(void* weapon, CharTexture* frameBuffer, ColorTexture* colorBuffer,  Camera* cam);
void updateWeapon(void* weapon);
void deleteWeapon(void* weapon);



#endif