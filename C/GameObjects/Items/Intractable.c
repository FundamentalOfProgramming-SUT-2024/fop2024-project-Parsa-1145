#include "Intractable.h"
#include "Action.h"
#include "../../ProgramStates/MainGame.h"
#include "../../SecondaryThread.h"

void initDoor(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->name = writeLog("Door");
    o->update = &updateDoor;
    o->render = &defaultItemRender;
    o->playerCollision = &doorPlayerCollision;
}
void initStair(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->name = writeLog("Stair");
    o->update = &defaultItemUpdate;
    o->render = &defaultItemRender;
    o->playerCollision = &stairPlayerCollision;
}
void initTrap(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->name = writeLog("Trap");
    o->update = &defaultItemUpdate;
    o->render = &defaultItemRender;
    o->playerCollision = &trapPlayerCollision;
}
void initPasswordGenerator(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->name = writeLog("Password generator");
    o->update = &updatePasswordGenerator;
    o->render = &defaultItemRender;
    o->playerCollision = &passwordGeneratorPlayerCollision;
    o->color[0] = 5;
    o->color[1] = 5;
    o->color[2] = 5;
}

void trapPlayerCollision(ItemBase* o){
    player.health -= 2;

    o->sprite = '^';
}
void doorPlayerCollision(ItemBase* o){
    if(o->sprite == '#'){
        o->sprite = '+';
        addMessage(writeLog("You found a hidden door"));
    }
    if(o->locked){
        addMessage(writeLog("Door is locked"));
    }
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

void generatePassword(ItemBase* o){
    ItemBase* door = findItemById(o->relId);
    if(!door->passwordValid){
        addMessage(writeLog("Generated password"));
        door->passwordValid = 1;
        door->password = rand() % 10000;
        addMessage(writeLog("The password is %d", door->password));
        addTimed1ObjCallbackCallback(&invalidatePassword, door, 10);
    }else{
        addMessage(writeLog("Cannot generate more right now"));
    }
}

void updatePasswordGenerator(ItemBase* o){
    if(isPlayerNextTo(o)){
        addInteraction("generate password", &generatePassword, 'c', o);
    }
}
void enterPassword(ItemBase* o){
    if(!o->lockBroken){
        if(o->passwordValid){
            addMessage(writeLog("Enter the password:"));
            while(o->passwordValid){
                int input = terminalGetInt();
                if(input == 0){
                    break;
                }else if(o->passwordValid){
                        if(o->password == input){
                        o->collider = 0;
                        o->locked = 0;
                        o->sprite = '$';
                        o->color[0] = 0;
                        o->color[1] = 5;
                        o->color[2] = 1;
                        addMessage(writeLog("Door unlocked", o->password));
                        break;
                    }else{
                        if(o->decayed == 0){
                            addFormattedMessage("%oThe lock has been damaged%O", 4, 2, 0);
                        }else if(o->decayed == 1){
                            addFormattedMessage("%oThe lock has been damaged%O", 5, 1, 0);
                        }else if(o->decayed == 2){
                            addFormattedMessage("%oThe lock broke%O", 5, 0, 0);
                            o->lockBroken = 1;
                            break;
                        }
                        o->decayed++;
                    }
                }else{
                    addMessage(writeLog("Password invalid"));
                    break;
                }
            }
        }else{
            addMessage(writeLog("Password invalid"));
        }
    }else{
        addFormattedMessage("%oThe lock is broken%O", 5, 0, 0);
    }
}
void updateDoor(ItemBase* o){
    if(o->collider && isPlayerNextTo(o)){
        addInteraction("enter password", &enterPassword, 'p', o);
    }
}
void invalidatePassword(ItemBase* o){
    if(!o->lockBroken){
        o->passwordValid = 0;
        addMessage(writeLog("Password was reset"));
        renderMainGame();
    }
}