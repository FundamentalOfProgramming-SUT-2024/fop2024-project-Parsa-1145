#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "MovingCell.h"
#include "../GlobalDefines.h"
#include"../Globals.h"

typedef int(*IterateCellType)(LinkedList* list, MovingCell* cell, Floor* f);

IterateCellType iterateCellType[1] = {&iterateRootCell};


int validForBranching(Floor* f, int x, int y){
    return ((f->groundMesh->data[y][x] == '\0') && (x < f->groundMesh->w) && (y < f->groundMesh->h));
}

int iterateRootCell(LinkedList* list, MovingCell* cell, Floor* f){
    int x = cell->x, y = cell->y;
    
    MovingCell* newCell;
    if((y >= f->groundMesh->h) || (x >= f->groundMesh->w)){
        return 0;
    }
    
    if(f->groundMesh->data[y][x] == '\0'){
        f->groundMesh->data[y][x] = '.';
        f->groundMesh->color[y][x] = rgb[2][2][2];
    }if((f->featureMesh->data[y][x] == cell->attr[5]) && (!cell->attr[6])){
        switch(cell->attr[7]){
            case 2:
                f->groundMesh->data[y][x] = 'L';
                break;
            case 1:
                f->groundMesh->data[y][x] = 'H';
                break;
            case 0:
                f->groundMesh->data[y][x] = 'D';
                break;
            default:
            break;
        }
        cell->attr[6] = 1;
    }

    {
        int tmp = 0;
        if(f->featureMesh->data[y+1][x] == cell->attr[4]){
            f->groundMesh->data[y+1][x] = 'D';
            free(cell);
            return 0;
        }
        if(f->featureMesh->data[y][x+1] == cell->attr[4]){
            f->groundMesh->data[y][x+1] = 'D';
            free(cell);
            return 0;
        }
        if(f->featureMesh->data[y-1][x] == cell->attr[4]){
            f->groundMesh->data[y-1][x] = 'D';
            free(cell);
            return 0;
        }
        if(f->featureMesh->data[y][x-1] == cell->attr[4]){
            f->groundMesh->data[y][x-1] = 'D';
            free(cell);
            return 0;
        }
    }
    float xdir = cell->attr[0] - cell->x, ydir = cell->attr[1] - cell->y, dist;
    dist = hypot(xdir, ydir);
    if(dist <= 1){
        free(cell);
        return 0;
    }

    if((cell->attr[6])){
        if(( cell->attr[2] > 2) && (randWithProb(cell->attr[2]) * 0.1 + (cell->attr[3]==2) * 0.1)){
            if(cell->attr[3] == 2){
                cell->attr[3] = randWithProb(0.5);   
            }else{
                if(randWithProb(0.3))cell->attr[3] = 2;
                else cell->attr[3] = !(cell->attr[3]);
            }
            cell->attr[2] = 0;
        }else{
            cell->attr[2]++;
        }
    }

    if(cell->attr[3] == 2){
        ydir /= dist;
        cell->y += ydir;
        xdir /= dist;
        cell->x += xdir;

    }if(cell->attr[3] == 1){
        if(xdir < 1){
            cell->attr[3] = 0;
        }
        xdir /= dist;
        if((f->groundMesh->data[(int)cell->y][(int)(cell->x + xdir)])!='\0'){
            xdir = 0;
        }
    }if(cell->attr[3] == 0){
        if(ydir < 1){
            cell->attr[3] = 1;
        }
        ydir /= dist;
        cell->y += ydir;
    }
    return 1;

}

int iterateMovingCells(LinkedList* list, Floor* f){
    MovingCell* cell;
    int sCopy = list->size;
    FOR(i, sCopy){
        cell = (MovingCell*)linkedListGetElement(list, i);
        if(!iterateCellType[((MovingCell*)linkedListGetElement(list, i))->type](list, cell, f)){
            linkedListDeleteElement(list, i);
            i--;
            sCopy--;
        }
    }
}