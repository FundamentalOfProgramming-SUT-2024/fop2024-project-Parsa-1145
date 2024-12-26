#ifndef ROOM
#define ROOM

typedef struct{
    int x, y;
    int w, h;

    int theme;
    
}Room;

void createRoom(Room* room, int x, int y, int w, int h);
void renderRoom(Room* room);

#endif