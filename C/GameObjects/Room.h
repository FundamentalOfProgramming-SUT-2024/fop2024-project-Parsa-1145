#ifndef ROOM
#define ROOM

#include "../Utilities/LinkedList.h"
#include "../Utilities/PointCloud.h"

typedef struct Room{
    int x, y, z;
    int w, h;

    int tmpx, tmpy, tmpx2, tmpy2;

    int theme;
    struct Room* top, *bottom, *right, *left;
    int neighbours;
    int visited;
    int stairCandidate;
    Point* point;
    int index;

}Room;

void deleteRoom(Room* room);

#endif