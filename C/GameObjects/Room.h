#ifndef ROOM
#define ROOM

typedef struct{
    int x, y;
    int w, h;

    int tmpx, tmpy, tmpx2, tmpy2;

    int theme;
    
}Room;

void createRoom(Room* room, int x, int y, int w, int h);
void renderRoom(Room* room, int x, int y);

#endif