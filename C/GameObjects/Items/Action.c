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
    if(!strcmp(name, "heal")) return &effectHeal;
    else if (!strcmp(name, "strenghten")) return &effectStrengthen;
    else if (!strcmp(name, "speedIncrease")) return &effectSpeedIncrease;
    else if (!strcmp(name, "healthRegen")) return &effectHealthRegen;
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
    for(void** i = o->effects.data; i; i = i[0]){
        tmpEffect = i[1];
        newEffect = calloc(1, sizeof(Effect));
        newEffect->amount = tmpEffect->amount;
        newEffect->type = copyString(tmpEffect->type);
        newEffect->duration = tmpEffect->duration;
        newEffect->func = tmpEffect->func;

        linkedListPushBack(&(player.effects), newEffect);
    }

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
            int x = player.x - itemPtr->x, y = player.y - itemPtr->y;
            if(x * x + y * y < 2){
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
                    if(o->quantity > 1){
                        o->quantity--;
                    }
                    else{
                        player.heldObject = NULL;
                        removeItemFromLinkedList(&(player.items), o);
                        defaultItemDelete(o);
                        checkEquiped();
                    }
                    addItemToInventory(broken);
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

void deleteEffect(Effect* effect){
    free(effect->type);
    free(effect);
}

void deleteInteraction(Interaction* a){
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