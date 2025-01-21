#include "Intractable.h"
#include "Action.h"
#include "../../ProgramStates/MainGame.h"

void initDoor(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &defaultItemUpdate;
    o->render = &defaultItemRender;
    o->playerCollision = &doorPlayerCollision;
}
void initStair(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &defaultItemUpdate;
    o->render = &defaultItemRender;
    o->playerCollision = &stairPlayerCollision;
}
void initTrap(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &defaultItemUpdate;
    o->render = &defaultItemRender;
    o->playerCollision = &trapPlayerCollision;
}
void initPasswordGenerator(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &defaultItemUpdate;
    o->render = &defaultItemRender;
    o->playerCollision = &trapPlayerCollision;
}


void trapPlayerCollision(ItemBase* o){

}
void doorPlayerCollision(ItemBase* o){

}

void stairPlayerCollision(ItemBase* o){
    ItemBase* dest = findItemById(o->relId);

    if(dest->z > o->z){
        addInteraction("go down", &moveInStair, KEY_DOWN, o);
    }else{
        addInteraction("go up", &moveInStair, KEY_UP, o);
    }
}


void passwordGeneratorPlayerCollision(ItemBase* o){
    
}