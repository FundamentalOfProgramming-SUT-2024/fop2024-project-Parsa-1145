#include <stdlib.h>

#include "Room.h"
#include "../GameObjects/Renderer.h"
#include "../GlobalDefines.h"
#include "../Globals.h"
#include "../ProgramStates/MainGame.h"

Room* createRoom( int x, int y, int w, int h){
    Room* out = malloc(sizeof(Room));
    out->x = x;
    out->y = y;
    out->h = h;
    out->w = w;

    return out;
}

void deleteRoom(Room* room){
    free(room);
}