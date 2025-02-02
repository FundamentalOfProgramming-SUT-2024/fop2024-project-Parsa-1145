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
    
    MovingCell* newCell;
    if((cell->y >= f->groundMesh->h) || (cell->x >= f->groundMesh->w)){
        return 0;
    }
    f->visited->data[cell->y][cell->x] = '.';
    if((f->featureMesh->data[cell->y][cell->x] == cell->attr[5]) && (!cell->attr[6])){
        switch(cell->attr[7]){
            case 2:
                f->groundMesh->data[cell->y][cell->x] = 'L';
                break;
            case 1:
                f->groundMesh->data[cell->y][cell->x] = 'H';
                break;
            case 0:
                f->groundMesh->data[cell->y][cell->x] = 'D';
                break;
            default:
            break;
        }
        cell->x += cell->attr[8];
        cell->y += cell->attr[9];
        f->groundMesh->data[cell->y][cell->x] = '.';
        cell->attr[6] = 1;
    }

    {
        int tmp = 0;
        if(f->featureMesh->data[cell->y][cell->x] == cell->attr[4]){
            f->groundMesh->data[cell->y][cell->x] = 'D';
            free(cell);
            return 0;
        }
        if(f->featureMesh->data[cell->y+1][cell->x] == cell->attr[4]){
            f->groundMesh->data[cell->y+1][cell->x] = 'D';
            free(cell);
            return 0;
        }
        if(f->featureMesh->data[cell->y][cell->x+1] == cell->attr[4]){
            f->groundMesh->data[cell->y][cell->x+1] = 'D';
            free(cell);
            return 0;
        }
        if(f->featureMesh->data[cell->y-1][cell->x] == cell->attr[4]){
            f->groundMesh->data[cell->y-1][cell->x] = 'D';
            free(cell);
            return 0;
        }
        if(f->featureMesh->data[cell->y][cell->x-1] == cell->attr[4]){
            f->groundMesh->data[cell->y][cell->x-1] = 'D';
            free(cell);
            return 0;
        }
    }


    if((cell->attr[6])){
        if(( cell->attr[2] > 2) && (randWithProb(cell->attr[2]) * 0.1)){
            cell->attr[3] = !(cell->attr[3]);
            cell->attr[2] = 0;
        }else{
            cell->attr[2]++;
        }
    }
    float xdir = cell->attr[0] - cell->x, ydir = cell->attr[1] - cell->y, dist;
    dist = hypot(xdir, ydir);
    if(dist <= 1.5){
        free(cell);
        return 0;
    }

    if(fabs(xdir) < 1.5) xdir = 0;
    else{
         if(xdir > 0) xdir = 1;
        else xdir = -1;
    }
    if(fabs(ydir) < 1.5) ydir = 0;
    else{
        if(ydir > 0) ydir = 1;
        else ydir = -1;
    }

    

    if((cell->attr[3] == 1)){
        if((pow(xdir * dist, 2) < 1)){
            cell->attr[3] = 0;
        }else{
            cell->x += xdir;
            if((f->groundMesh->data[cell->y][cell->x] == '#') && ((f->featureMesh->data[cell->y][cell->x] != cell->attr[4]) && (f->featureMesh->data[cell->y][cell->x] != cell->attr[5]))){
                cell->x -= xdir;
                if(cell->attr[9]){
                    cell->y += cell->attr[9];
                }else{
                    cell->y += randWithProb(0.5) * 2 - 1;
                }
            }
        }
    }else{
        xdir = 0;   
    }
    if(f->groundMesh->data[cell->y][cell->x] == '\0'){
        f->groundMesh->data[cell->y][cell->x] = '.';
    }
    if((cell->attr[3] == 0)){
        if((pow(ydir * dist, 2) < 1)){
            cell->attr[3] = 1;
        }else{
            cell->y += ydir;
            if((f->groundMesh->data[cell->y][cell->x] == '#') && ((f->featureMesh->data[cell->y][cell->x] != cell->attr[4]) && (f->featureMesh->data[cell->y][cell->x] != cell->attr[5]))){
                cell->y -= ydir;
                if(cell->attr[8]){
                    cell->x += cell->attr[8];
                }else{
                    cell->x += randWithProb(0.5) * 2 - 1;
                }
            }
        }
    }else{
        ydir = 0;
    }
    if(f->groundMesh->data[cell->y][cell->x] == '\0'){
        f->groundMesh->data[cell->y][cell->x] = '.';
    }
    cell->attr[8] = xdir;
    cell->attr[9] = ydir;
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