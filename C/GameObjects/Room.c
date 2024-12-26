#include "Room.h"
#include "../GlobalDefines.h"
#include "../Globals.h"

void createRoom(Room* room, int x, int y, int w, int h){
    room->x = x;
    room->y = y;
    room->h = h;
    room->w = w;
}
void renderRoom(Room* room, int x, int y){
    room->tmpx = max(room->x - x, 0); room->tmpy = max(room->y - y, 0); 
    room->tmpx2 = min(scrW-1, room->x + room->w - 1 - x); room->tmpy2 = min(scrH-1, room->y + room->h - 1 - y);
    for(int i = room->tmpy; i <= room->tmpy2 ;i++){
        move(i, room->tmpx);
        for(int j = room->tmpx; j <= room->tmpx2;j++){
            addch('.');
        }
    }
}