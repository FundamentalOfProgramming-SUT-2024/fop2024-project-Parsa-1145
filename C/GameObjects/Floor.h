#ifndef FLOOR
#define FLOOR

#include"Room.h"

typedef struct{
    int minx, miny, maxx, maxy;

    Room* roomList;
    int roomNum;
    char** groundMesh;
}Floor;

#endif