#include "Monster.h"
#include "../../ProgramStates/MainGame.h"
#include "../Renderer.h"
#include <math.h>

int defaultMonsterRender(ItemBase* m, CharTexture* frameBuffer, Camera* cam){
    if(isinRect(m->x, m->y, cam->x, cam->y, cam->w, cam->h)){
        if(!castRay(m->x, m->y, player.x, player.y,floors + player.z, 5, NULL)){
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
        if(ptr->collider && (ptr->x == x) && (ptr->y == y)){
            return 0;
        }
    }
    return 1;

}
int validForVisit(PathPoint* p, int x, int y, Floor* f){
    x = p->x + x;
    y = p->y + y;
    return ((x < f->w) && (y < f->h) && (x >= 0) && (y >= 0) && (!f->pathFindMesh->data[y][x]) && (f->groundMesh->data[y][x] == '.'));
}

int directions[2][4][2] = {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}, {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
PathPoint* pathFind(int x1, int y1, int x2, int y2, Floor* f){
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

    //CharTexture* fb = createCharTexture(scrW, scrH);

    while(iterPtr){
        curPoint = (iterPtr)[1];
        //fillCharTexture(fb, '\0');
        dirSeed = randWithProb(0.5);
        FOR(i, 4){
            if(validForVisit(curPoint, directions[dirSeed][i][0], directions[dirSeed][i][1], f)){
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
        // erase();
        // renderTexture(f->pathFindMesh, 0, 0, &mainCamera, fb);
        // renderFrameBuffer(fb);
        // refresh();
        
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
int defaultMonsterUpdate(ItemBase* m){
    if(0){
        if(!castRay(m->x, m->y, player.x, player.y,floors + player.z, 10, NULL)){
            m->decayed = m->decayTime;
        }
        if(m->decayed){
            PathPoint* tmpPoint = pathFind(m->x, m->y, player.x, player.y, floors + player.z);
            if(tmpPoint){
                if(tmpPoint[0].depth > 2){
                    if((hypot(tmpPoint[2].x - m->x, tmpPoint[2].y - m->y) < 2) && validForMove(tmpPoint[2].x, tmpPoint[2].y, floors + player.z)){
                        m->x = tmpPoint[2].x;
                        m->y = tmpPoint[2].y;
                    }else if (validForMove(tmpPoint[1].x, tmpPoint[1].y, floors + player.z)){
                        m->x = tmpPoint[1].x;
                        m->y = tmpPoint[1].y;
                    }
                    // m->x = tmpPoint[1].x;
                    //     m->y = tmpPoint[1].y;
                }else{
                    addMessage(writeLog("Damagoiaos!!!"));
                }
                free(tmpPoint);

            }
            m->decayed--;
        }

    }

    
}
