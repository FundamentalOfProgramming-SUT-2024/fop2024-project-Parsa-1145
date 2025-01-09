#ifndef MOVINGCELL
#define MOVINGCELL
#include "LinkedList.h"
#include "../GameObjects/Texture.h"
#include "../GameObjects/Floor.h"

typedef struct{
    int type;
    int attr[10];
    
    float x, y;
} MovingCell;

int iterateRootCell(LinkedList* list, MovingCell* cell, Floor* f);
//int iteratebranchCell(LinkedList* list, MovingCell* cell, Floor* f);
int iterateMovingCells(LinkedList* list, Floor* f);

#endif