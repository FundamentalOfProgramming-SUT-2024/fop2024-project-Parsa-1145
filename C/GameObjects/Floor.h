#ifndef FLOOR
#define FLOOR

#include"Room.h"
#include "Texture.h"

typedef struct{
    int minx, miny, maxx, maxy;

    Room** roomList;
    int roomNum;
    
    CharTexture* groundMesh;
    ColorTexture* groundMeshColor;
}Floor;

#endif