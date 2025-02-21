#include <string.h>
#include "Monster.h"
#include "../../ProgramStates/MainGame.h"
#include "../Renderer.h"
#include <math.h>

void initMonster(ItemBase* m){
    m->render = &defaultItemRender;
    m->update = &defaultMonsterUpdate;
    m->primaryUse = &defaultMonsterAttack;

    m->collider = 1;
    m->goodness = 0;
}
int canSeePlayer(ItemBase* m){
    return !((castRay(m->x, m->y, player.x, player.y,floors + player.z, m->visionRadius, NULL)) || (player.invisible));
}
int defaultMonsterRender(ItemBase* m, CharTexture* frameBuffer, Camera* cam){
    if(isinRect(m->x, m->y, cam->x, cam->y, cam->w, cam->h)){
        if(canSeePlayer(m)){
            m->color[0] = 0;
            m->color[1] = 5;
            m->color[2] = 0;
        }else{
            m->color[0] = 5;
            m->color[1] = 0;
            m->color[2] = 0;
        }
        
        frameBuffer->data[m->y - cam->y][m->x - cam->x] = m->sprite;
        frameBuffer->color[m->y - cam->y][m->x - cam->x] = rgb[m->color[0]][m->color[1]][m->color[2]];
    }
}


typedef struct PathPoint{
    int x, y, depth;
    struct PathPoint* prev;
}PathPoint;

int validForMove(int x, int y, Floor* f){
    void** tmp = f->itemList->data;
    ItemBase* ptr;
    if(f->groundMesh->data[y][x] != '.'){
        return 0;
    }
    while(tmp){
        ptr = tmp[1];
        tmp = tmp[0];
        if(ptr->collider && (ptr->x == x) && (ptr->y == y) && ((!ptr->type || (ptr->type && strcmp(ptr->type, "monster"))) || ptr->decayed)){
            return 0;
        }
    }
    return 1;

}
int validForVisit(PathPoint* p, int x, int y, Floor* f, int o){
    x = p->x + x;
    y = p->y + y;
    return ((x < f->w) && (y < f->h) && (x >= 0) && (y >= 0) && (!f->pathFindMesh->data[y][x]) && ((!o && (f->groundMesh->data[y][x] == '.')) || (o &&(validForMove(x, y, f)))));
}

int directions[2][4][2] = {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}, {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
PathPoint* pathFind(int x1, int y1, int x2, int y2, Floor* f, int o){
    fillCharTexture(f->pathFindMesh, '\0');
    LinkedList* path = malloc(sizeof(LinkedList));
    createLinkedList(path, sizeof(PathPoint*));

    PathPoint* curPoint, *tmpPoint, *out;
    void** iterPtr;
    int found = 0, index;
    int dirSeed = randWithProb(0.5);

    curPoint = malloc(sizeof(PathPoint));
    curPoint->x = x1;
    curPoint->y = y1;
    curPoint->prev = NULL;
    curPoint->depth = 0;
    f->pathFindMesh->data[y1][x1] = 1;

    linkedListPushBack(path, curPoint);

    iterPtr = path->data;

    while(iterPtr){
        curPoint = (iterPtr)[1];
        dirSeed = randWithProb(0.5);
        FOR(i, 4){
            if(validForVisit(curPoint, directions[dirSeed][i][0], directions[dirSeed][i][1], f, o)){
                tmpPoint = malloc(sizeof(PathPoint));
                tmpPoint->x = curPoint->x + directions[dirSeed][i][0];
                tmpPoint->y = curPoint->y + directions[dirSeed][i][1];
                f->pathFindMesh->data[tmpPoint->y][tmpPoint->x] = 'a';
                tmpPoint->prev = curPoint;
                tmpPoint->depth = curPoint->depth+1;
                linkedListPushBack(path, tmpPoint);
            }
        }

        if((curPoint->x == x2) && (curPoint->y == y2)){
            found = 1;
            break;
        }
        
        iterPtr = iterPtr[0];
    }

    if(found){
        out = malloc(sizeof(PathPoint) * (curPoint->depth + 1));
        out[0].depth = curPoint->depth;
        while(curPoint){
            out[curPoint->depth].x = curPoint->x;
            out[curPoint->depth].y = curPoint->y;
            curPoint = curPoint->prev;
        }
    }

    iterPtr = path->data;
    while(iterPtr){
        free(iterPtr[1]);
        iterPtr = iterPtr[0];
    }
    free(path);
    if(found){
        return out;
    }else return NULL;
}


void defaultMonsterAttack(ItemBase* m){
    if(randWithProb(m->openingProb)){
        if(randWithProb(0.5)){
            addFormattedMessage("%S layed a %ohit%O on you", m->name, 5, 1, 1);
        }else{
            addFormattedMessage("%S inflicts %odamage%O", m->name, 5, 1, 1);
        }

        player.health -= m->damage;

        
        if(player.health <= 0){
            if(randWithProb(0.5)){
                endGame(0, writeLog("You were slain by a %s.", m->name));
            }else{
                endGame(0, writeLog("A %s has defeated you.", m->name));
            }
        }
    }else{
        if(randWithProb(0.5)){
            addFormattedMessage("%S barely %omisses%O", m->name, 1, 5, 1);
        }else{
            addFormattedMessage("%S attacked but you %odoged%O", m->name, 1, 5, 1);
        }
    }
}
int defaultMonsterUpdate(ItemBase* m){
    if(deltaTime){
        switch(m->goodness){
            case 0:
                if(canSeePlayer(m)){
                    if(hypot(player.x - m->x, player.y - m->y) < 1.5){
                        m->primaryUse(m);
                    }
                    if(!m->cursed){
                        addFormattedMessage("%o%S saw you%O", 5, 1, 1, m->name);
                        m->tarx = player.x;
                        m->tary = player.y;
                        m->goodness = 1;
                        m->decayed = m->decayTime;
                    }
                }
                break;
            case 1:
                if((player.z == m->z) && (!player.invisible) && !(m->cursed)){
                    PathPoint* tmpPoint = pathFind(m->x, m->y, player.x, player.y, floors + player.z, 1);
                    if(!tmpPoint) tmpPoint = pathFind(m->x, m->y, player.x, player.y, floors + player.z, 0);
                    if(tmpPoint){
                        if(hypot(player.x - m->x, player.y - m->y) > 1.5){
                            if (validForMove(tmpPoint[1].x, tmpPoint[1].y, floors + player.z)){
                                m->x = tmpPoint[1].x;
                                m->y = tmpPoint[1].y;
                                m->decayed--;
                            }
                        }else{
                            m->primaryUse(m);
                        }
                        free(tmpPoint);
                    }
                    
                    if(!m->decayed){
                        if((floors[m->z].featureMesh->data[m->y][m->x] == 3)){
                            m->decayed = 10;
                        }else{
                            m->decayed = m->decayTime / 2;
                            if(randWithProb(0.5)){
                                m->goodness = 2;
                                addFormattedMessage("%S is %oexhausted%O", m->name, 1, 5, 1 );
                            }else{
                                m->goodness = 3;
                                addFormattedMessage("%S lost %ointrest%O in you", m->name, 1, 5, 1 );
                            }
                        }
                    }
                }else{
                    m->goodness = 0;
                }
                break;
            case 2:
                m->decayed--;
                if(!m->decayed) m->goodness = 0;
                break;
            case 3:
                PathPoint* tmpPoint = pathFind(m->x, m->y, m->tarx, m->tary, floors + m->z, 1);
                if(tmpPoint && (tmpPoint[0].depth > 1)){
                    m->x = tmpPoint[1].x;
                    m->y = tmpPoint[1].y;
                    free(tmpPoint);
                }else{
                    m->goodness = 0;
                }

                break;
        }
    }

    
}

void defaultMonsterDeath(ItemBase* m){
    if(randWithProb(0.5)){
        addFormattedMessage("%oThe %S died%O", 1, 4, 1, m->name);
    }else{
        addFormattedMessage("%oYou killed the %S%O", 1, 4, 1, m->name);
    }

    if(m->deathSound){
        playEffect(m->deathSound);
    }
    defaultItemDelete(m);
    removeItemFromLinkedList(floors[player.z].itemList, m);
}
void defaultMonsterTakeDamage(ItemBase* m, ItemBase* o, int damage){
    switch (randBetween(0, 2, 0)){
    case 0:
        playEffectByName("monsterHit2");
        break;
    case 1:
        playEffectByName("monsterHit1");
        break;
    default:
        break;
    }

    m->health -= damage;

    if(m->health <= 0){
        defaultMonsterDeath(m);
    }
}