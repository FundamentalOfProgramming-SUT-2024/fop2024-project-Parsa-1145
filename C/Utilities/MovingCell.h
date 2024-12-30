#ifndef MOVINGCELL
#define MOVINGCELL
#include "LinkedList.h"
#include "../GameObjects/Texture.h"

typedef struct{
    int type;
    int attr[5];
    
    float x, y;
} MovingCell;

int iterateRootCell(LinkedList* list, MovingCell* cell, CharTexture* ground);
int iteratebranchCell(LinkedList* list, MovingCell* cell, CharTexture* ground);
int iterateMovingCells(LinkedList* list, CharTexture* ground);

#endif