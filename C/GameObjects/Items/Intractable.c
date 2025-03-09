#include "Intractable.h"
#include "Action.h"
#include "../../ProgramStates/MainGame.h"
#include "../../SecondaryThread.h"

void initDoor(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &updateDoor;
    o->render = &defaultItemRender;
    o->playerCollision = &doorPlayerCollision;
}
void initTressureRoomEntrance(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->playerCollision = &tressureRoomEntrancePlayerCollision;
    o->render = &defaultItemRender;
}
void initStair(ItemBase* o){
    if(!(o->id)){
        o->id = globalItemIdCounter++;
    }
    o->update = &stairUpdate;
    o->render = &defaultItemRender;
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
    o->update = &updatePasswordGenerator;
    o->render = &defaultItemRender;
    o->playerCollision = &passwordGeneratorPlayerCollision;
    o->color[0] = 5;
    o->color[1] = 5;
    o->color[2] = 5;
}

void trapPlayerCollision(ItemBase* o){
    if(o->hidden){
        if(!player.levitating){
            o->hidden = 0;
            o->primaryUse(o);
        }
    }else{
        addMessage(writeLog("You avoided the trap"));
    }
}
void doorPlayerCollision(ItemBase* o){
    if(o->hidden){
        floors[player.z].groundMesh->data[o->y][o->x] = '.';
        o->hidden = 0;
        addMessage(writeLog("You found a hidden door"));
    }
    if(o->locked){
        addMessage(writeLog("Door is locked"));
    }
}

void stairUpdate(ItemBase* o){
    ItemBase* dest = findItemById(o->relId);

    if((player.x == o->x) && (player.y == o->y)){
        if(dest->z > o->z){
            addInteraction("go down", &moveInStair, KEY_DOWN, o);
        }else{
            addInteraction("go up", &moveInStair, KEY_UP, o);
        }
    }
}
void tressureRoomEntrancePlayerCollision(ItemBase* o){
    addInteraction("enter the amulet chamber", &enterAmuletChamber, KEY_DOWN, o);
}
void passwordGeneratorPlayerCollision(ItemBase* o){
    
}

void generatePassword(ItemBase* o){
    ItemBase* door = findItemById(o->relId);
    if(!door->passwordValid){
        addMessage(writeLog("Generated password"));
        door->passwordValid = 1;
        while(door->password < 1000) door->password = rand() % 10000;

        if(randWithProb(0.3)){
            door->cursed = 1;
        }else{
            door->cursed = 0;
        }
        addMessage(writeLog("The password is %d", door->password));
        addTimed1ObjCallbackCallback(&invalidatePassword, door, 10000);
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
                    int correct = 0;
                    if(!o->cursed){
                        if(o->password == input) correct = 1;
                    }else{
                        char* tmp1 = writeLog("%d", o->password);
                        char* tmp2 = writeLog("%d", input);

                        correct = 1;
                        FOR(i, 4){
                            if(tmp1[i] != tmp2[3-i]){
                                correct = 0;
                            }
                        }

                        free(tmp1);
                        free(tmp2);
                    }
                    if(correct){
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
                        renderMainGame();
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