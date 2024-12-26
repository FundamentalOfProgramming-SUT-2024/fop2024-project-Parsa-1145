#include "Room.h"
#include "../GlobalDefines.h"
#include "../Globals.h"

void createRoom(Room* room, int x, int y, int w, int h){
    room->x = x;
    room->y = y;
    room->h = h;
    room->w = w;
}
void renderRoom(Room* room){
    FOR(i, room->h){
        move(room->y + i, room->x);
        FOR(j, room->w){
            addch('0');
        }
    }
}