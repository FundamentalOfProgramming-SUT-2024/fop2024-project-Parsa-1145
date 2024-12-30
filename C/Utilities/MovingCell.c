#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "MovingCell.h"
#include "../GlobalDefines.h"
#include"../Globals.h"

typedef int(*IterateCellType)(LinkedList* list, MovingCell* cell, CharTexture* ground);

IterateCellType iterateCellType[2] = {&iterateRootCell, &iteratebranchCell};


int validForBranching(CharTexture* ground, int x, int y){
    return ((ground->data[y][x] == '\0') && (x < ground->w) && (y < ground->h));
}

int iterateRootCell(LinkedList* list, MovingCell* cell, CharTexture* ground){
    if(cell->attr[0] != NULL){
        int x = cell->x, y = cell->y;
        
        MovingCell* newCell;

        if(validForBranching(ground, x+1, y)){
            newCell = malloc(sizeof(MovingCell));
            newCell->x = x + 1;
            newCell->y = y;
            newCell->attr[0] = randBetween(0, 2, 0);
            newCell->type = 1;
        ground->data[(int)newCell->y][(int)newCell->x] = '.';

            linkedListPushBack(list, newCell);
        }if(validForBranching(ground, x - 1, y)){
            newCell = malloc(sizeof(MovingCell));
            newCell->x = x - 1;
            newCell->y = y;
            newCell->attr[0] = randBetween(0, 2, 1);
            newCell->type = 1;
        ground->data[(int)newCell->y][(int)newCell->x] = '.';

            linkedListPushBack(list, newCell);

        }if(validForBranching(ground, x, y + 1)){
            newCell = malloc(sizeof(MovingCell));
            newCell->x = x;
            newCell->y = y + 1;
            newCell->attr[0] = randBetween(0, 2, 2);
            newCell->type = 1;
        ground->data[(int)newCell->y][(int)newCell->x] = '.';

            linkedListPushBack(list, newCell);

        }if(validForBranching(ground, x, y - 1)){
            newCell = malloc(sizeof(MovingCell));
            newCell->x = x;
            newCell->y = y - 1;
            newCell->attr[0] = randBetween(0, 2, 3);
            newCell->type = 1;
        ground->data[(int)newCell->y][(int)newCell->x] = '.';

            linkedListPushBack(list, newCell);
        }
        if((y >= ground->h) || (x >= ground->w)){
            return 0;
        }
        //ground->data[y][x] = '.';
        float xdir = cell->attr[0] - cell->x, ydir = cell->attr[1] - cell->y, dist;
        dist = hypot(xdir, ydir);
        if(dist <= 1.5){
            free(cell);
            return 0;
        }
        xdir /= dist;
        ydir /= dist;
        cell->x += xdir;
        cell->y += ydir;

        return 1;
    }


}

int iteratebranchCell(LinkedList* list, MovingCell* cell, CharTexture* ground){
    if(cell->attr[0] == 0){
        return 0;
    }
    int x = cell->x, y = cell->y;
    MovingCell* newCell;
    if(validForBranching(ground, x+1, y)){
        newCell = malloc(sizeof(MovingCell));
        newCell->x = x+1;
        newCell->y = y;
        newCell->attr[0] = cell->attr[0] - 1;
        newCell->type = 1;
        ground->data[(int)newCell->y][(int)newCell->x] = '.';

        linkedListPushBack(list, newCell);
    }if(validForBranching(ground, x - 1, y)){
        newCell = malloc(sizeof(MovingCell));
        newCell->x = x - 1;
        newCell->y = y;
        newCell->attr[0] = cell->attr[0] - 1;
        newCell->type = 1;
        ground->data[(int)newCell->y][(int)newCell->x] = '.';

        linkedListPushBack(list, newCell);

    }if(validForBranching(ground, x, y + 1)){
        newCell = malloc(sizeof(MovingCell));
        newCell->x = x;
        newCell->y = y + 1;
        newCell->attr[0] = cell->attr[0] - 1;
        newCell->type = 1;
        ground->data[(int)newCell->y][(int)newCell->x] = '.';
        linkedListPushBack(list, newCell);

    }if(validForBranching(ground, x, y - 1)){
        newCell = malloc(sizeof(MovingCell));
        newCell->x = x;
        newCell->y = y - 1;
        newCell->attr[0] = cell->attr[0] - 1;
        newCell->type = 1;
        ground->data[(int)newCell->y][(int)newCell->x] = '.';
        linkedListPushBack(list, newCell);
    }

    if((y >= ground->h) || (x >= ground->w)){
        return 0;
    }
    
    free(cell);
    return 0;
}
int iterateMovingCells(LinkedList* list, CharTexture* ground){
    MovingCell* cell;
    int sCopy = list->size;
    FOR(i, sCopy){
        cell = linkedListGetElement(list, i);
        if(!iterateCellType[((MovingCell*)linkedListGetElement(list, i))->type](list, cell, ground)){
            linkedListDeleteElement(list, i);
            i--;
            sCopy--;
        }
    }
}