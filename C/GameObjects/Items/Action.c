#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#include "Action.h"
#include "../../ProgramStates/MainGame.h"
#include "../Renderer.h"


void(*getAction(char* name))(ItemBase*){
    if(!strcmp(name, "consume")) return &consume;
    else if (!strcmp(name, "Unlock door")) return &unlockDoor;
    else if (!strcmp(name, "Cast spell")) return &castSpell;
    else if (!strcmp(name, "Shoot arrow")) return &shootArrow;
    else if (!strcmp(name, "Stab")) return &stab;
    else if (!strcmp(name, "Swing attack")) return &swingAttack;
    else if (!strcmp(name, "Throw")) return &throwAttack;
    else if (!strcmp(name, "Kick")) return &kick;
    else if (!strcmp(name, "trapSimpleDamage")) return &trapSimpleDamage;
    else if (!strcmp(name, "trapPoisonDamage")) return &trapPoisonDamage;
    else if (!strcmp(name, "trapFallToNextFloor")) return &trapFallToNextFloor;
    else if (!strcmp(name, "trapTeleport")) return &trapTeleport;
    else if (!strcmp(name, "takeAim")) return &takeAim;

    else return NULL;
}
void(*getEffectFunc(char* name))(Effect*){
    if(!strcmp(name, "Heal")) return &effectHeal;
    else if (!strcmp(name, "Strenghten")) return &effectStrengthen;
    else if (!strcmp(name, "Speed increase")) return &effectSpeedIncrease;
    else if (!strcmp(name, "Health regeneration")) return &effectHealthRegen;
    else if (!strcmp(name, "Fill")) return &effectFill;
    else if (!strcmp(name, "Maxhealth increase")) return &effectMaxHealthIncrease;
    else if (!strcmp(name, "Poison")) return &effectPoison;
    else if (!strcmp(name, "Levitation")) return &effectLevitation;
    else if (!strcmp(name, "Burning")) return &effectBurning;
    else if (!strcmp(name, "Great Luck")) return &effectGreatLuck;
    else if (!strcmp(name, "Invisibility")) return &effectInvisibility;

    else return NULL;
}
void moveInStair(ItemBase* o){
    ItemBase* dest = findItemById(o->relId);

    player.x = dest->x;
    player.y = dest->y;
    player.z = dest->z;
}

int takeAim(ItemBase* o){
    RayCollision collision;
    castRay(player.x, player.y, mainCamera.x + mEvent.x, mainCamera.y + mEvent.y, floors + player.z, player.heldObject->range, &collision);
    renderLine(0, rgb[1][5][2], UINT8_MAX, player.x, player.y, collision.x, collision.y, &mainCamera, frameBuffer);
}

int consume(ItemBase* o){
    Effect* newEffect;
    Effect* tmpEffect;

    addMessage(writeLog("You consumed the %s", o->name));
    if((!strcmp(o->subType, "food")) && o->goodness == 0){
        addMessage(writeLog("The %s was rotten", o->name));
    }
    for(void** i = o->effects.data; i; i = i[0]){
        tmpEffect = i[1];
        newEffect = calloc(1, sizeof(Effect));
        newEffect->amount = tmpEffect->amount;
        newEffect->type = copyString(tmpEffect->type);
        newEffect->duration = tmpEffect->duration;
        newEffect->func = tmpEffect->func;

        addPlayerEffect(newEffect);
    }
    updateWorld(0, 0);

    updateEffectsTab();
}
int swingAttack(ItemBase* o){
    void** tmp = floors[player.z].itemList->data;
    ItemBase* ptr;
    while(tmp){
        ptr = tmp[1];
        tmp = tmp[0];

        if((ptr->type) && !strcmp(ptr->type, "monster")){
            if(hypot(player.x - ptr->x, player.y - ptr->y) < 1.5){
                ptr->health -= player.heldObject->damage;

                if(randWithProb(player.heldObject->openingProb * player.luck)){
                    if(randWithProb(0.5)){
                        addFormattedMessage("You dealt %o%D%O damage to %S", 1, 4, 1, player.heldObject->damage, ptr->name);
                    }else{
                        addFormattedMessage("%o%D%O damage inflicted on %S", 1, 4, 1, player.heldObject->damage, ptr->name);
                    }
                    if(ptr->health <= 0){
                        addFormattedMessage("The %S %odied%O", ptr->name, 1, 4, 1);
                        defaultItemDelete(ptr);
                        removeItemFromLinkedList(floors[player.z].itemList, ptr);
                    }
                }else{
                    if(randWithProb(0.5)){
                        addFormattedMessage("You %omissed%O the %S", 4, 1, 1, ptr->name);
                    }else{
                        addFormattedMessage("%s %odoged%O your attack", ptr->name, 4, 1, 1);
                    }
                }
            }
        }
    }
}
int stab(ItemBase* o){
    void** tmp = floors[player.z].itemList->data;
    ItemBase* ptr;
    while(tmp){
        ptr = tmp[1];
        tmp = tmp[0];

        if((ptr->type) && !strcmp(ptr->type, "monster")){
            if(hypot(player.x - ptr->x, player.y - ptr->y) < 1.5){
                ptr->health -= player.heldObject->damage;

                if(randWithProb(player.heldObject->openingProb * player.luck)){
                    if(randWithProb(0.5)){
                        addFormattedMessage("You dealt %o%D%O damage to %S", 1, 4, 1, player.heldObject->damage, ptr->name);
                    }else{
                        addFormattedMessage("%o%D%O damage inflicted on %S", 1, 4, 1, player.heldObject->damage, ptr->name);
                    }
                    if(ptr->health <= 0){
                        addFormattedMessage("The %S %odied%O", 1, 4, 1, ptr->name);
                        defaultItemDelete(ptr);
                        removeItemFromLinkedList(floors[player.z].itemList, ptr);
                    }
                }else{
                    if(randWithProb(0.5)){
                        addFormattedMessage("You %omissed%O the %S", 4, 1, 1, ptr->name);
                    }else{
                        addFormattedMessage("%s %odoged%O your attack", ptr->name, 4, 1, 1);
                    }
                }
            }
        }
    }
}
int castSpell(ItemBase* o){
    addMessage(writeLog("HOHO cast spell"));
}
int kick(ItemBase* o){
    addMessage(writeLog("HOHO kick"));
}
int throwAttack(ItemBase* o){
    int ch;
    int exit = 0;
    while(!exit){
        ch = getch();
        switch(ch){
            case KEY_RESIZE:
                getmaxyx(stdscr, scrH, scrW);
                clear();
                refresh();

                mainCamera.h = scrH;
                mainCamera.w = scrW;

                resizeCharTexture(&frameBuffer, mainCamera.w, mainCamera.h);
                resizeCharTexture(&visitedMaskBuffer, mainCamera.w, mainCamera.h);
                break;
            case KEY_MOUSE:
                if(getmouse(&mEvent) == OK){
                    switch(mEvent.bstate){
                    case KEY_MOUSE_MOVE:
                        break;
                    default:
                        if(mEvent.bstate & BUTTON3_PRESSED){
                            exit = 1;
                        }else if(mEvent.bstate & BUTTON1_PRESSED){
                            if(o->quantity == 1){
                                o->inInventory = 0;
                                removeItemFromLinkedList(&(player.items), o);
                                checkEquiped();
                                updateInventoryTab();
                            }else{
                                o->quantity--;
                                o = LoadItemWithName(o->name);
                                o->quantity = 1;

                                o->inInventory = 0;
                                o->x = player.x;
                                o->z = player.z;
                                o->y = player.y;
                            }

                            linkedListPushBack(floors[player.z].itemList, o);

                            RayCollision c;
                            int collision = castRay(player.x, player.y, mainCamera.x + mEvent.x, mainCamera.y + mEvent.y, floors + player.z, o->range, &c);

                            if(collision){
                                c.x = c.prevx;
                                c.y = c.prevy;
                            }

                            float xdir = c.x - player.x, ydir = c.y - player.y, curx = player.x, cury = player.y;

                            float dist = hypot(xdir, ydir);
                            xdir /= dist;
                            ydir /= dist;

                            if(dist > 1.5){
                                dist = 0;
                                while(1){
                                    sleepMili(20000);
                                    curx += xdir;
                                    cury += ydir;
                                    o->x = round(curx);
                                    o->y = round(cury);
                                    
                                    dist++;

                                    if(hypot(c.x - curx, c.y - cury) <= 0.5){
                                        break;
                                    }else if(dist > o->range){
                                        break;
                                    }else{
                                        renderMainGame();
                                    }
                                }

                            }

                            return 0;
                        }
                        break;
                    }
                }
                break;
            default:
                break;
                
        }
        renderMainGameToFramebuffer();

        RayCollision collision;
        //castRay(player.x, player.y, mainCamera.x + mEvent.x, mainCamera.y + mEvent.y, floors + player.z, 5, &coli);
        castRay(player.x, player.y, mainCamera.x + mEvent.x, mainCamera.y + mEvent.y, floors + player.z, player.heldObject->range, &collision);
        renderLine(0, rgb[1][5][2], -1, player.x, player.y, collision.x, collision.y, &mainCamera, frameBuffer);

        renderMainGameToTerminal();
    }
}
int shootArrow(ItemBase* o){
    addMessage(writeLog("HOHO Shoot arrow"));
}
int trapSimpleDamage(ItemBase* o){
    player.health -= o->damage;
    addFormattedMessage("%oYou stepped on a spike%O", 5, 1, 1);
}
int trapPoisonDamage(ItemBase* o){
    Effect* poison = malloc(sizeof(Effect));
    poison->duration = 10;
    poison->amount = 1;
    poison->type = writeLog("Poison");
    poison->func = getEffectFunc("Poison");
    poison->elapsed = 0;
    addPlayerEffect(poison);
    addFormattedMessage("%oYou stepped on a poisoned spike%O", 1, 4, 1);
}
int trapFallToNextFloor(ItemBase* o){
    player.z++;
    placeInRange(floors + player.z, 0, 0, floors[player.z].w, floors[player.z].h, &(player.x), &(player.y));
    addFormattedMessage("%oYou fell down from a hole%O", 5, 4, 3);
    player.health -= 20;
    if(player.health <= 0){
        endGame(0, writeLog("You fell down, seems it was fatal."));
    }
}
int trapTeleport(ItemBase* o){
    placeInRange(floors + player.z, 0, 0, floors[player.z].w, floors[player.z].h, &(player.x), &(player.y));
    addFormattedMessage("%oYou where teleported by a trap%O", 1, 3, 5);
    player.health -= 15;
    if(player.health <= 0){
        endGame(0, writeLog("You couldnt bare the teleportation."));
    }
}
int unlockDoor(ItemBase* o){
    ItemBase* itemPtr;
    void** tmpPtr = floors[player.z].itemList->data;
    while(tmpPtr){
        itemPtr = tmpPtr[1];
        tmpPtr = tmpPtr[0];
        if((itemPtr->name) && (!strcmp(itemPtr->name, "Door")) && (itemPtr->locked)){
            if(isPlayerNextTo(itemPtr)){
                if(!itemPtr->lockBroken){
                    if(randWithProb(o->openingProb)){
                        addMessage(writeLog("Door unlocked"));
                        itemPtr->locked = 0;
                        itemPtr->collider = 0;
                        itemPtr->color[0] = 1;
                        itemPtr->color[1] = 5;
                    }else{
                        addMessage(writeLog("Key broke"));
                        ItemBase* broken;
                        broken = LoadItemWithName("Broken key");
                        broken->quantity = 1;
                        addItemToInventory(broken);
                        useItem(o);
                    }
                }else{
                    addMessage(writeLog("The lock is broken"));
                }
            }
        }
    }
}

int effectSpeedIncrease(struct Effect* e){
    player.speedModifier = e->amount; 
}
int effectHealthRegen(struct Effect* e){
     player.healthRegenModifier = e->amount;
}
int effectStrengthen(struct Effect* e){
    player.strengthModifier = e->amount;
}
int effectHeal(struct Effect* e){
    player.health = min(player.maxHealth, player.health += e->amount);
}
int effectFill(Effect* e){
    player.fullness = min(player.baseFullness, player.fullness + e->amount);
}
int effectSicken(Effect*){
    player.healthRegenModifier = 0;
}
int effectMaxHealthIncrease(Effect* e){
    player.baseMaxHealth += e->amount;
}
int effectGreatLuck(Effect* e){
    player.luckModifier *= e->amount;
}
int effectInvisibility(Effect* e){
    player.invisible = 1;
}
int effectPoison(Effect* e){
    if(deltaTime){
        player.health -= e->amount;
        if(player.health <= 0){
            endGame(0, writeLog("A fatal poison killed you."));
        }
    }
}
int effectBurning(Effect* e){
    if(deltaTime){
        player.health -= e->amount;
        if(player.health <= 0){
            endGame(0, writeLog("You burned to death."));
        }
    }
}
int effectLevitation(Effect*){
    player.levitating = 1;
}
void deleteEffect(Effect* effect){
    free(effect->type);
    free(effect);
}
void deleteInteraction(Interaction* a){
    free(a->name);
    free(a);
}
void addInteraction(char* name, void (*func)(ItemBase*),  int key, ItemBase* o){
    Interaction* new = malloc(sizeof(Interaction));

    new->func = func;
    new->key = key;
    new->object = o;
    new->name = copyString(name);

    linkedListPushBack(&playerActionList, new);
}
cJSON* effectToJson(Effect* e){
    cJSON* out = cJSON_CreateObject();
    cJSON_AddStringToObject(out, "type", e->type);
    cJSON_AddNumberToObject(out, "amount", e->amount);
    cJSON_AddNumberToObject(out, "duration", e->duration);
    cJSON_AddNumberToObject(out, "elapsed", e->elapsed);

    return out;
}
Effect* loadEffect(cJSON* data){
    Effect* new = malloc(sizeof(Effect));
    new->duration = 0;
    if(cJSON_GetObjectItem(data, "type")){
        new->type = copyString(cJSON_GetObjectItem(data, "type")->valuestring);
        new->func = getEffectFunc(new->type);
    }if(cJSON_GetObjectItem(data, "amount")){
        new->amount = cJSON_GetObjectItem(data, "amount")->valuedouble;
    }if(cJSON_GetObjectItem(data, "duration")){
        new->duration = cJSON_GetObjectItem(data, "duration")->valueint;
    }if(cJSON_GetObjectItem(data, "elapsed")){
        new->elapsed = cJSON_GetObjectItem(data, "elapsed")->valueint;
    }
    return new;
}