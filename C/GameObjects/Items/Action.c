#include <string.h>

#include "Action.h"
#include "../../ProgramStates/MainGame.h"

void(*getAction(char* name))(ItemBase*){
    if(!strcmp(name, "consume")) return &consume;
    else if (!strcmp(name, "Unlock door")) return &unlockDoor;
    else if (!strcmp(name, "Cast spell")) return &castSpell;
    else if (!strcmp(name, "Shoot arrow")) return &shootArrow;
    else if (!strcmp(name, "Stab")) return &stab;
    else if (!strcmp(name, "Swing attack")) return &swingAttack;
    else if (!strcmp(name, "Throw")) return &throwAttack;
    else if (!strcmp(name, "Kick")) return &kick;
    else return NULL;
}
void(*getEffectFunc(char* name))(Effect*){
    if(!strcmp(name, "Heal")) return &effectHeal;
    else if (!strcmp(name, "Strenghten")) return &effectStrengthen;
    else if (!strcmp(name, "Speed increase")) return &effectSpeedIncrease;
    else if (!strcmp(name, "Health regeneration")) return &effectHealthRegen;
    else if (!strcmp(name, "Fill")) return &effectFill;
    else if (!strcmp(name, "Maxhealth increase")) return &effectMaxHealthIncrease;
    else return NULL;
}
void moveInStair(ItemBase* o){
    ItemBase* dest = findItemById(o->relId);

    player.x = dest->x;
    player.y = dest->y;
    player.z = dest->z;

    updateWorld(0, 0);
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
    addMessage(writeLog("HOHO Swing"));
}
int stab(ItemBase* o){
    addMessage(writeLog("stab the shit out of it"));
}
int castSpell(ItemBase* o){
    addMessage(writeLog("HOHO cast spell"));
}
int kick(ItemBase* o){
    addMessage(writeLog("HOHO kick"));
}
int throwAttack(ItemBase* o){
    addMessage(writeLog("HOHO thwrow that bitch"));
}
int shootArrow(ItemBase* o){
    addMessage(writeLog("HOHO Shoot arrow"));
}
int unlockDoor(ItemBase* o){
    ItemBase* itemPtr;
    void** tmpPtr = floors[player.z].itemList->data;
    while(tmpPtr){
        itemPtr = tmpPtr[1];
        tmpPtr = tmpPtr[0];
        if((!strcmp(itemPtr->name, "Door")) && (itemPtr->locked)){
            if(isPlayerNextTo(itemPtr)){
                if(!itemPtr->lockBroken){
                    if(randWithProb(o->openingProb)){
                        addMessage(writeLog("Door unlocked"));
                        itemPtr->locked = 0;
                        itemPtr->collider = 0;
                        itemPtr->sprite = '+';
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