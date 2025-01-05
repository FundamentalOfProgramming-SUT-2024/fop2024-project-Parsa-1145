#ifndef ROOM
#define ROOM

#include "../Utilities/LinkedList.h"
#include "../Utilities/PointCloud.h"
typedef struct Room{
    int x, y;
    int w, h;

    int tmpx, tmpy, tmpx2, tmpy2;

    int theme;
    struct Room* top, *bottom, *right, *left;
    int visited;
    Point* point;
    int index;

}Room;

Room* createRoom(int x, int y, int w, int h);
void deleteRoom(Room* room);

#endif