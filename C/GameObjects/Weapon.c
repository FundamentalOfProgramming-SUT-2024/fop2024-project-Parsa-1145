#include <stdlib.h>

#include "Weapon.h"
#include "../Globals.h"

void createWeapon(Weapon* weapon){
    weapon->gameObject = malloc(sizeof(GameObject));

    weapon->gameObject->x = &(weapon->x);
    weapon->gameObject->y = &(weapon->y);
    weapon->gameObject->z = &(weapon->z);

    weapon->gameObject->object = weapon;

    weapon->gameObject->render = &renderWeapon;
    weapon->gameObject->drop = &dropWeapon;
    weapon->gameObject->pickUp = &pickUpWeapon;
    weapon->gameObject->update = &updateWeapon;
    weapon->gameObject->deleteObject = &deleteWeapon;
}
void pickUpWeapon(void* weapon){

}
void dropWeapon(void* weapon){

}
void renderWeapon(void* weapon, CharTexture* frameBuffer, ColorTexture* colorBuffer, Camera* cam){
    Weapon* w = (Weapon*) weapon;

    if(isinRect(w->x, w->y, cam->x, cam->y, cam->w, cam->h)){
        frameBuffer->data[w->y - cam->y][w->x - cam->x] = w->sprite;
    }
}
void updateWeapon(void* weapon){

}
void deleteWeapon(void* weapon){
    Weapon* w = (Weapon*) weapon;

    free(w->gameObject);
    free(w);

}