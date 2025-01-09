#include <stdlib.h>

#include "Room.h"
#include "../GameObjects/Renderer.h"
#include "../GlobalDefines.h"
#include "../Globals.h"
#include "../ProgramStates/MainGame.h"

void deleteRoom(Room* room){
    free(room);
}