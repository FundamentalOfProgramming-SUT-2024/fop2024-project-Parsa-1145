#ifndef ROOM
#define ROOM

#include "../Utilities/LinkedList.h"
typedef struct{
    int x, y;
    int w, h;

    int tmpx, tmpy, tmpx2, tmpy2;

    int theme;

    LinkedList* items;
    
}Room;

Room* createRoom(int x, int y, int w, int h);
void deleteRoom(Room* room);

#endif