#ifndef ROOM
#define ROOM

typedef struct{
    int x, y;
    int w, h;

    int tmpx, tmpy, tmpx2, tmpy2;

    int theme;

    int visited;
    
}Room;

Room* createRoom(int x, int y, int w, int h);

#endif